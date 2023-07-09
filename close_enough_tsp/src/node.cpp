//
// Created by Dominik Krupke on 12.12.22.
//

#include "cetsp/node.h"
namespace cetsp {

static bool is_segments_intersect(const Point &p11, const Point &p12,
                                  const Point &p21, const Point &p22);

void Node::add_lower_bound(const double lb) {
  if (get_lower_bound() < lb) {
    lazy_lower_bound_value = lb;
    // propagate to parent
    if (parent != nullptr && parent->get_lower_bound() < lb) {
      parent->reevaluate_children();
    }
    // Potentially also propagate to children.
    if (!children.empty()) {
      for (auto &child : children) {
        child->add_lower_bound(lb);
      }
    }
  }
}

auto Node::get_lower_bound() -> double {
  if (!lazy_lower_bound_value) {
    lazy_lower_bound_value = get_relaxed_solution().obj();
    if (parent != nullptr) {
      if (lazy_lower_bound_value < parent->get_lower_bound()) {
        lazy_lower_bound_value = parent->get_lower_bound();
      }
    }
  }
  return *lazy_lower_bound_value;
}

bool Node::is_feasible() { return _relaxed_solution.is_feasible(); }

void Node::branch(std::vector<std::shared_ptr<Node>> &children_) {
  if (is_pruned()) {
    throw std::invalid_argument("Cannot branch on pruned node.");
  }
  assert(!is_feasible());
  if (children_.empty()) {
    prune();
    children = std::vector<std::shared_ptr<Node>>{};
  } else {
    children = children_;
    reevaluate_children();
  }
}

auto Node::get_relaxed_solution() -> const PartialSequenceSolution & {
  return _relaxed_solution;
}

void Node::prune(bool infeasible) {
  if (pruned) {
    return;
  }
  pruned = true;
  if (infeasible) {
    add_lower_bound(std::numeric_limits<double>::infinity());
  }
  for (auto &child : children) {
    child->prune(infeasible);
  }
}

void Node::reevaluate_children() {
  if (!children.empty()) {
    auto lb = std::transform_reduce(
        children.begin(), children.end(),
        std::numeric_limits<double>::infinity(),
        [](double a, double b) { return std::min(a, b); },
        [](std::shared_ptr<Node> &node) { return node->get_lower_bound(); });
    add_lower_bound(lb);
  }
}

std::vector<TrajectoryIntersection> Node::get_intersections() {
  /* currently only tours are supported */
  assert(!instance->is_path());
  const auto &solution = get_relaxed_solution();
  const auto &seq = solution.get_sequence();

  /* Collect all edges */
  std::vector<
      std::tuple<const Point &, const Point &, const Circle &, const Circle &>>
      edges;
  for (unsigned int i = 0; i < seq.size(); i++) {
    unsigned int j = (i + 1) % seq.size();
    const Circle &c1 = (*instance).at(seq[i]);
    const Circle &c2 = (*instance).at(seq[j]);
    const Point &p1 = solution.get_sequence_hitting_point(i);
    const Point &p2 = solution.get_sequence_hitting_point(j);
    edges.push_back(std::make_tuple(p1, p2, c1, c2));
  }

  /* Search for intersections */
  std::vector<TrajectoryIntersection> intersections;
  for (unsigned int i = 0; i < edges.size(); i++) {
    for (unsigned int j = 0; j < edges.size(); j++) {
      unsigned int i_prev = ((int)i - 1) % edges.size();
      unsigned int i_next = (i + 1) % edges.size();
      if (j == i_prev || j == i || j == i_next)
        continue;
      auto const &a = edges[i];
      auto const &b = edges[j];
      if (is_segments_intersect(std::get<0>(a), std::get<1>(a), std::get<0>(b),
                                std::get<1>(b))) {
        intersections.push_back(TrajectoryIntersection(
            std::get<0>(a), std::get<1>(a), std::get<2>(a), std::get<3>(a),
            std::get<0>(b), std::get<1>(b), std::get<2>(b), std::get<3>(b)));
      }
    }
  }
  return intersections;
}

static bool is_segments_intersect(const Point &p11, const Point &p12,
                                  const Point &p21, const Point &p22) {
  auto ccw = [](const Point &a, const Point &b, const Point &c) {
    return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
  };

  return ccw(p11, p21, p22) != ccw(p12, p21, p22) &&
         ccw(p11, p12, p21) != ccw(p11, p12, p22);
}

} // namespace cetsp
