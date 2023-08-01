/**
 * The partial sequence solution is a fundamental part of the BnB.
 * It essentially represents the relaxed solution, but also takes care of the
 * nasty computation of the optimal tour through the circles.
 * Using this class, you can easily build you own BnB-algorithm.
 * It is important that some of the operators are lazy.
 */

#ifndef CETSP_RELAXED_SOLUTION_H
#define CETSP_RELAXED_SOLUTION_H
#include "common.h"
#include "details/distance_cache.h"
#include "details/lazy_trajectory.h"
#include <vector>

namespace cetsp {

class PartialSequenceSolution {
  /**
   * This class simplifies the handling of the relaxed solution that is based
   * on a sequence of circles, that allow to trigger_lazy_computation the
   * optimal tour respecting this order using a second order cone program.
   */
public:
  PartialSequenceSolution(const Instance *instance, std::vector<int> sequence_,
                          double feasibility_tol = 0.001)
      : spanning_trajectory(instance, std::move(sequence_)), instance{instance},
        FEASIBILITY_TOL{feasibility_tol}, distances{instance} {
    const auto &sequence = spanning_trajectory.sequence;
    if (sequence.empty() && !instance->is_path()) {
      throw std::invalid_argument("Cannot trigger_lazy_computation tour "
                                  "trajectory from empty sequence.");
    }
    assert(std::all_of(sequence.begin(), sequence.end(), [&instance](auto i) {
      return i < static_cast<int>(instance->size());
    }));
  }

  bool trigger_lazy_computation(bool with_feasibility = false) const {
    const auto fresh = spanning_trajectory.trigger_computation();
    if (with_feasibility) {
      auto feas = is_feasible();
    }
    return fresh;
  }

  /**
   * Returns true if the i-th circle in the sequence is spanning. This
   * information is useful to simplify the solution.
   * @param i The index of the circle within the sequence. Not the index of  the
   * circle in the solution.
   * @return True if it spans the trajectory.
   */
  bool is_sequence_index_spanning(int i) const {
    trigger_lazy_computation();
    return spanning_trajectory.get_spanning_information()[i];
  }

  const Point &trajectory_begin() const {
    return get_trajectory().points.front();
  }

  const Point &trajectory_end() const { return get_trajectory().points.back(); }

  /**
   * Returns the point that covers the i-th circle in the sequence. These
   * are the potential turning points in the trajectory.
   * @param i
   * @return
   */
  const Point &get_sequence_hitting_point(int i) const {
    if (instance->is_tour()) {
      return get_trajectory().points[i];
    } else {
      return get_trajectory().points[i + 1];
    }
  }

  const Trajectory &get_trajectory() const {
    return spanning_trajectory.get_trajectory();
  }

  const std::vector<int> &get_sequence() const {
    return spanning_trajectory.sequence;
  }

  double obj() const { return get_trajectory().length(); }

  double distance(int i) const { return distances(i, &get_trajectory()); }

  bool covers(int i) const;

  bool is_feasible() const;

  /**
   * Simplify the sequence and the solution by removing implicitly covered
   * parts.
   */
  void simplify();

protected:
  details::LazyTrajectoryComputation spanning_trajectory;

private:
  const Instance *instance;
  mutable std::optional<bool> _feasible;
  bool simplified = false;
  mutable int feasible_below = 0;
  double FEASIBILITY_TOL;
  mutable details::DistanceCache distances;
};

class Solution : public PartialSequenceSolution {
public:
  Solution(const Instance *instance, std::vector<int> sequence_,
           double feasibility_tol = 0.001)
      : PartialSequenceSolution(instance, sequence_, feasibility_tol) {
    assert(is_feasible());
  }

  Solution(const Instance *instance, std::vector<int> sequence_,
           Trajectory traj, std::vector<bool> &spanning_info,
           double feasibility_tol = 0.001)
      : PartialSequenceSolution(instance, sequence_, feasibility_tol) {
    spanning_trajectory.update(traj, spanning_info);
    assert(is_feasible());
  }

  Solution(const PartialSequenceSolution &sol) : PartialSequenceSolution(sol) {
    assert(is_feasible());
  }

  Solution(PartialSequenceSolution &&sol)
      : PartialSequenceSolution(std::move(sol)) {
    assert(is_feasible());
  }
};

} // namespace cetsp

#endif // CETSP_RELAXED_SOLUTION_H
