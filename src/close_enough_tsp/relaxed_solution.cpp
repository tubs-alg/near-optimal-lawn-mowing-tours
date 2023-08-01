#include "cetsp/relaxed_solution.h"
#include "cetsp/soc.h"
//
// Created by Dominik Krupke on 15.01.23.
//

namespace cetsp::details {
void LazyTrajectoryComputation::compute_trajectory() const {
  if (instance->is_path()) {
    compute_path_trajectory();
  } else {
    compute_tour_trajectory();
  };
}
void LazyTrajectoryComputation::compute_tour_trajectory() const {
  // compute the optimal tour trajectory through the sequence
  std::vector<Circle> circles;
  circles.reserve(sequence.size());
  for (auto i : sequence) {
    assert(i < static_cast<int>(instance->size()));
    circles.push_back((*instance).at(i));
  }
  assert(circles.size() == sequence.size());
  auto soc = compute_trajectory_with_information(circles, false);
  data = std::move(soc);
}
void LazyTrajectoryComputation::compute_path_trajectory() const {
  // compute the optimal path trajectory through the sequence
  std::vector<Circle> circles;
  circles.reserve(sequence.size() + 2);
  circles.emplace_back(instance->path->first, 0);
  for (auto i : sequence) {
    circles.push_back((*instance).at(i));
  }
  circles.emplace_back(instance->path->second, 0);
  assert(circles.size() == sequence.size() + 2);
  auto soc = compute_trajectory_with_information(circles, true);
  const int n = static_cast<int>(soc.second.size());
  for (int i = 1; i < n - 1; ++i) {
    soc.second[i - 1] = soc.second[i];
  }
  soc.second.pop_back(); // remove last element
  data = std::move(soc);
}

} // namespace cetsp::details
void cetsp::PartialSequenceSolution::simplify() {
  if (simplified) {
    return;
  }
  std::vector<Point> points;
  std::vector<int> simplified_sequence;
  std::vector<bool> is_spanning;
  if (instance->is_path()) {
    // Trajectory of a path has a fixed beginning, not represented in the
    // sequence
    points.push_back(trajectory_begin());
  }
  // add all spanning circles and their hitting points
  const auto &sequence = spanning_trajectory.sequence;
  for (int i = 0; i < sequence.size(); ++i) {
    if (is_sequence_index_spanning(i)) {
      points.push_back(get_sequence_hitting_point(i));
      simplified_sequence.push_back(sequence[i]);
      is_spanning.push_back(true);
    }
  }
  // Close the trajectory
  if (instance->is_path()) {
    // Trajectory of a path has a fixed ending, not represented in the
    // sequence
    points.push_back(trajectory_end());
  } else {
    // close the tour by going back to  the beginning
    points.push_back(points.front());
  }
  // update the trajectory and sequence. Feasibility etc. doesn't change.
  spanning_trajectory.sequence = std::move(simplified_sequence);
  spanning_trajectory.update(Trajectory(points), std::move(is_spanning));
  simplified = true;
}
bool cetsp::PartialSequenceSolution::is_feasible() const {
  if (_feasible && !*_feasible) {
    return false;
  }
  if (!_feasible || *_feasible) {
    _feasible = true;
    // will be cached if the instance hasn't changed. Otherwise, only the
    // unchecked instances will be checked.
    int n = static_cast<int>(instance->size());
    for (; feasible_below < n; ++feasible_below) {
      if (!covers(feasible_below)) {
        _feasible = false;
        break;
      }
    }
  }
  return *_feasible;
}
bool cetsp::PartialSequenceSolution::covers(int i) const {
  const auto &sequence = spanning_trajectory.sequence;
  if (std::any_of(sequence.begin(), sequence.end(),
                  [i](const auto &j) { return i == j; })) {
    return true;
  }
  return distance(i) <= FEASIBILITY_TOL;
}
