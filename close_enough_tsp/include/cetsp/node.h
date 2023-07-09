/**
 * Implements a node in the BnB tree.
 */
#ifndef CETSP_NODE_H
#define CETSP_NODE_H
#include "cetsp/common.h"
#include "cetsp/soc.h"
#include "relaxed_solution.h"
#include <memory>
#include <numeric>
#include <optional>
namespace cetsp {

/**
 * Represent an intersection in a specific trajectory.
 * The intersection is between two edges of the cycles c1c2, c3c4 which have the
 * exact coordinates p1p2, p3p4
 */
struct TrajectoryIntersection {
public:
  Point p1, p2, p3, p4;
  Circle c1, c2, c3, c4;
  TrajectoryIntersection(Point p1, Point p2, Circle c1, Circle c2, Point p3,
                         Point p4, Circle c3, Circle c4)
      : p1(p1), p2(p2), c1(c1), c2(c2), p3(p3), p4(p4), c3(c3), c4(c4) {}
};

class Node {
public:
  Node(Node &node) = delete;
  Node(Node &&node) = default;
  explicit Node(std::vector<int> branch_sequence_, Instance *instance,
                Node *parent = nullptr)
      : _relaxed_solution(instance, std::move(branch_sequence_)),
        parent{parent}, instance{instance} {
    if (parent != nullptr) {
      _depth = parent->depth() + 1;
    }
  }

  void trigger_lazy_evaluation() {
    _relaxed_solution.trigger_lazy_computation(true);
  }

  void add_lower_bound(double lb);

  auto get_lower_bound() -> double;

  bool is_feasible();

  void branch(std::vector<std::shared_ptr<Node>> &children_);

  [[nodiscard]] const std::vector<std::shared_ptr<Node>> &get_children() const {
    return children;
  }
  [[nodiscard]] std::vector<std::shared_ptr<Node>> &get_children() {
    return children;
  }

  [[nodiscard]] Node *get_parent() { return parent; }
  [[nodiscard]] const Node *get_parent() const { return parent; }

  auto get_relaxed_solution() -> const PartialSequenceSolution &;

  /**
   * Will prune the node, i.e., mark it as not leading to an optimal solution
   * and thus stopping at it. Pruned nodes are allowed to be deleted from
   * memory.
   */
  void prune(bool infeasible = true);

  [[nodiscard]] const std::vector<int> &get_fixed_sequence() {
    return _relaxed_solution.get_sequence();
  }

  /**
   * The spanning sequence is a subset of the fixed sequence, but with
   * all indices belonging to circles that to not span/define the trajectroy
   * removed.
   * @return The orded list of indices of the circles spanning the current
   * trajectory.
   */
  [[nodiscard]] std::vector<int> get_spanning_sequence() {
    std::vector<int> spanning_sequence;
    int n = static_cast<int>(_relaxed_solution.get_sequence().size());
    spanning_sequence.reserve(n);
    for (int i = 0; i < n; ++i) {
      if (_relaxed_solution.is_sequence_index_spanning(i)) {
        spanning_sequence.push_back(_relaxed_solution.get_sequence()[i]);
      }
    }
    return spanning_sequence;
  }

  void simplify() { _relaxed_solution.simplify(); }

  [[nodiscard]] auto is_pruned() const -> bool { return pruned; }

  [[nodiscard]] Instance *get_instance() { return instance; }

  [[nodiscard]] int depth() const { return _depth; }

  std::vector<TrajectoryIntersection> get_intersections();

private:
  // Check if the children allow to improve the lower bound.
  void reevaluate_children();

  PartialSequenceSolution _relaxed_solution;
  std::optional<double> lazy_lower_bound_value{};
  std::vector<std::shared_ptr<Node>> children;
  Node *parent;

  int _depth = 0;
  bool pruned = false;
  Instance *instance;
};
} // namespace cetsp
#endif // CETSP_NODE_H
