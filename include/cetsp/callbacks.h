//
// Look here to  see how you can influence the behavior of
// the BnB algorithm.
//

#ifndef CETSP_CALLBACKS_H
#define CETSP_CALLBACKS_H
#include "cetsp/details/solution_pool.h"
#include "cetsp/strategies/branching_strategy.h"
#include "cetsp/strategies/root_node_strategy.h"
#include "cetsp/strategies/search_strategy.h"
namespace cetsp {

struct EventContext {
  /**
   * This class provides you the information within a callback.
   * It also allows you to modify the behavior of the BnB.
   */
  std::shared_ptr<Node> current_node; // The node being investigated
  std::shared_ptr<Node> root_node;    // The root of the BnB tree
  Instance *instance;                 //  The instance being solved.
  SolutionPool *solution_pool;        // The already found solutions.
  int num_iterations;                 // number of nodes already investigated.

  /**
   * Add a lazy constraint. This has to be deterministic and
   * be satisified by all already found solutions.
   */
  void add_lazy_circle(Circle &circle) { instance->add_circle(circle); }

  /**
   * Add a feasible solution. This may help to prune a lot
   * of branches.
   */
  void add_solution(Solution &sol) { solution_pool->add_solution(sol); }

  /**
   * Returns the global(!) lower bound. Use `current_node` to
   * access the lower bound of the node.
   */
  double get_lower_bound() const { return root_node->get_lower_bound(); }

  /**
   * Returns the global upper bound.
   */
  double get_upper_bound() const { return solution_pool->get_upper_bound(); }

  /**
   * Returns true if the currently considered node would be
   * feasible according to the currently added constraints.
   * May be useful to query in combination with adding lazy
   * constraints.
   */
  bool is_feasible() const { return current_node->is_feasible(); }

  /**
   * Returns the relaxed solution of the currently investigated
   * node. Use `node->get_lower_bound()` to query the  lower bound
   * and do not  trigger_lazy_computation the length of the relaxed solution.
   */
  const auto &get_relaxed_solution() const {
    return current_node->get_relaxed_solution();
  }

  /**
   * Returns the currently best known feasible solution,
   * if available.
   */
  std::unique_ptr<Solution> get_best_solution() const {
    return solution_pool->get_best_solution();
  }
};

class B2BNodeCallback {

  /**
   * Interface for implementing callbacks that can
   * influence the BnB by, e.g., improving the lower bounds.
   */
public:
  B2BNodeCallback(){};
  virtual ~B2BNodeCallback() = default;

  virtual void on_entering_node(EventContext &) {
    /**
     * This would be a good place to improve the lower bounds
     * of the current node.
     */
  }
  virtual void add_lazy_constraints(EventContext &) {
    /**
     * This would be a good place to add lazy constraints for
     * a currently feasible solution. Note that this function
     * is only called if the investigated node satisfies all
     * current constraints.
     */
  }
  virtual void on_leaving_node(EventContext &) {
    /**
     * This is a good place to collect some statistics or
     * provide some nice printed output, as you can see
     * what happened to the node (pruned/branched/feasible).
     */
  }
};
};     // namespace cetsp
#endif // CETSP_CALLBACKS_H
