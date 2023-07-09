//
// Created by Dominik Krupke on 11.12.22.
//

#include "cetsp/common.h"
#include "cetsp/relaxed_solution.h"
#include "cetsp/soc.h"
#include <algorithm>
#include <iostream>
#include <random>
namespace cetsp {

bool swap_improves(std::vector<std::pair<Circle, int>> &circles, int i, int j) {
  assert(i < j);
  int prev_i = (i == 0 ? circles.size() - 1 : i - 1);
  int next_j = (j + 1) % circles.size();
  if (prev_i == j || next_j == i) {
    return false;
  }
  const auto prev_dist =
      circles[i].first.center.dist(circles[prev_i].first.center) +
      circles[j].first.center.dist(circles[next_j].first.center);
  const auto new_dist =
      circles[i].first.center.dist(circles[next_j].first.center) +
      circles[j].first.center.dist(circles[prev_i].first.center);
  return new_dist < 0.999 * prev_dist;
}

Solution compute_tour_by_2opt(Instance &instance) {
  auto rd = std::random_device{};
  auto rng = std::default_random_engine{rd()};
  std::vector<std::pair<Circle, int>> circles;
  int i = 0;
  for (const auto &c : instance) {
    circles.push_back({c, i});
    i++;
  }
  std::shuffle(std::begin(circles), std::end(circles), rng);
  bool changed = true;
  const auto n = circles.size();
  while (changed) {
    changed = false;
    for (unsigned i = 0; i < n; i++) {
      for (unsigned j = 0; j < i; j++) {
        assert(j < i);
        if (swap_improves(circles, j, i)) {
          std::reverse(circles.begin() + j, circles.begin() + i + 1);
          changed = true;
        }
      }
    }
  }
  // TODO: This is ugly as it does not care for begin and end.
  std::vector<int> sequence;
  for (const auto &c : circles) {
    sequence.push_back(c.second);
  }
  Solution sol(&instance, sequence);
  sol.simplify();
  return sol;
}
} // namespace cetsp
