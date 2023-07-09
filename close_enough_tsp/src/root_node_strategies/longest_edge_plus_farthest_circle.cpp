#include "cetsp/strategies/root_node_strategy.h"

namespace cetsp {
std::pair<int, int> find_max_pair(const std::vector<Circle> &instance) {
  /**
   * Find the circle pair with the longest distance  between its centers.
   */
  double max_dist = 0;
  std::pair<int, int> best_pair;
  for (unsigned i = 0; i < instance.size(); i++) {
    for (unsigned j = 0; j < i; j++) {
      auto dist = instance[i].center.squared_dist(instance[j].center);
      if (dist >= max_dist) {
        best_pair = {i, j};
        max_dist = dist;
      }
    }
  }
  return best_pair;
}

auto most_distanced_circle(const Instance &instance) {
  assert(instance.path);
  auto p0 = instance.path->first;
  auto p1 = instance.path->second;
  auto max_el = std::max_element(
      instance.begin(), instance.end(), [&](const Circle &a, const Circle &b) {
        return p0.dist(a.center) + p1.dist(a.center) <
               p0.dist(b.center) + p1.dist(b.center);
      });
  return std::distance(instance.begin(), max_el);
}
std::shared_ptr<Node>
LongestEdgePlusFurthestCircle::get_root_node(Instance &instance) {
  /**
   * Compute a  root note consisting of three circles by first finding
   * the most distanced pair and then adding a third circle that has the
   * longest sum of  distance to the two end points.
   */
  if (instance.is_path()) {
    if (instance.empty()) {
      return std::make_shared<Node>(std::vector<int>{}, &instance);
    }
    std::vector<int> seq;
    seq.push_back(static_cast<int>(most_distanced_circle(instance)));
    return std::make_shared<Node>(seq, &instance);
  } else {
    if (instance.size() <= 3) { // trivial case
      std::vector<int> seq;
      for (int i = 0; i < static_cast<int>(instance.size()); ++i) {
        seq.push_back(i);
      }
      return std::make_shared<Node>(seq, &instance);
    }
    auto max_pair = find_max_pair(instance);
    const auto c1 = instance[max_pair.first];
    const auto c2 = instance[max_pair.second];
    auto c3 = max_pair.first;
    double max_dist = 0;
    for (unsigned i = 0; i < instance.size(); ++i) {
      const auto &c = instance[i];
      auto dist = c1.center.dist(c.center) + c2.center.dist(c.center);
      if (dist > max_dist) {
        max_dist = dist;
        c3 = i;
      }
    }
    assert(max_pair.first < static_cast<int>(instance.size()));
    assert(max_pair.second < static_cast<int>(instance.size()));
    assert(c3 < static_cast<int>(instance.size()));
    return std::make_shared<Node>(
        std::vector<int>{max_pair.first, c3, max_pair.second}, &instance);
  }
}
} // namespace cetsp
