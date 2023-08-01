//
// Created by Dominik Krupke on 15.01.23.
//

#ifndef CETSP_LAZY_TRAJECTORY_H
#define CETSP_LAZY_TRAJECTORY_H

#include "../common.h"
#include <vector>
namespace cetsp::details {
class LazyTrajectoryComputation {
public:
  LazyTrajectoryComputation(const Instance *instance_,
                            std::vector<int> sequence_)
      : instance{instance_}, sequence{std::move(sequence_)} {}

  Trajectory &get_trajectory() const {
    trigger_computation();
    return data->first;
  }

  std::vector<bool> &get_spanning_information() const {
    trigger_computation();
    return data->second;
  }

  void update(Trajectory &trajectory, std::vector<bool> &spanning_info) {
    data = std::make_pair(trajectory, spanning_info);
  }

  void update(Trajectory &&trajectory, std::vector<bool> &&spanning_info) {
    data = std::make_pair(std::move(trajectory), std::move(spanning_info));
  }

  bool trigger_computation() const {
    if (data) {
      return false;
    }
    compute_trajectory();
    return true;
  }

  const Instance *instance;
  std::vector<int> sequence;

private:
  void compute_trajectory() const;

  void compute_tour_trajectory() const;

  void compute_path_trajectory() const;

  mutable std::optional<std::pair<Trajectory, std::vector<bool>>> data;
};
} // namespace cetsp::details
#endif // CETSP_LAZY_TRAJECTORY_H
