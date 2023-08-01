//
// Created by Dominik Krupke on 14.12.22.
// This file provide logic of keeping track of the solutions found and
// the corresponding upper bounds.
//

#ifndef CETSP_SOLUTION_POOL_H
#define CETSP_SOLUTION_POOL_H

#include "../common.h"
#include "../relaxed_solution.h"

namespace cetsp {
class SolutionPool {
public:
  void add_solution(const Solution &solution) {
    auto solution_length = solution.get_trajectory().length();
    if (solution_length < ub) {
      solutions.push_back(solution);
      ub = solution_length;
    }
  }
  double get_upper_bound() { return ub; }

  std::unique_ptr<Solution> get_best_solution() {
    if (solutions.empty()) {
      return nullptr;
    }
    return std::make_unique<Solution>(
        solutions.back()); // best solution is always at the end
  }

  bool empty() { return solutions.empty(); }

private:
  double ub = std::numeric_limits<double>::infinity();
  std::vector<Solution> solutions;
};
} // namespace cetsp
#endif // CETSP_SOLUTION_POOL_H
