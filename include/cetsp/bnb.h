/**
 * This file implements the Branch and Bound algorithm. The sub-strategies
 * are separated in other classes to make it easily adaptable.
 * With the user callbacks, you can influence it strongly.
 */

#ifndef CETSP_BNB_H
#define CETSP_BNB_H
#include "cetsp/callbacks.h"
#include "cetsp/details/solution_pool.h"
#include "cetsp/strategies/branching_strategy.h"
#include "cetsp/strategies/root_node_strategy.h"
#include "cetsp/strategies/search_strategy.h"
#include "cetsp/utils/timer.h"
#include "node.h"
#include <chrono>
namespace cetsp {

class BranchAndBoundAlgorithm {
  /**
   * Implements the branch and bound algorithm.
   */
public:
  BranchAndBoundAlgorithm(Instance *instance, std::shared_ptr<Node> root_,
                          BranchingStrategy &branching_strategy,
                          SearchStrategy &search_strategy)
      : instance{instance}, root{std::move(root_)},
        search_strategy{search_strategy},
        branching_strategy(branching_strategy) {
    branching_strategy.setup(instance, root, &solution_pool);
    search_strategy.init(root);
  }

  void add_node_callback(std::unique_ptr<B2BNodeCallback> &&callback) {
    assert(callback != nullptr);
    node_callbacks.push_back(std::move(callback));
  }

  /**
   * Add a feasible solution as upper bound. Note that it must also obey
   * all lazy constraints. This can speed up the algorithm as it can allow BnB
   * to prune a lot of suboptimal branches. You can add as many solutions as
   * you want as only the best is used.
   * @param solution The feasible solution.
   */
  void add_upper_bound(const Solution &solution) {
    solution_pool.add_solution(solution);
  }

  /**
   * Add a lower bound to the BnB-tree. This usually does not help much, it
   * may only be a benefit, it is higher than any LB found by BnB, but it does
   * probably not influence the algorithm itself as long as it is not very close
   * to the optimum.
   * @param lb The lower bound.
   */
  void add_lower_bound(double lb) { root->add_lower_bound(lb); }

  /**
   * Returns the current best known upper bound.
   */
  double get_upper_bound() { return solution_pool.get_upper_bound(); }
  /**
   * Returns the current best known lower bound.
   * @return
   */
  double get_lower_bound() { return root->get_lower_bound(); }

  /**
   * Returns the currently best solution, if one exists.
   * @return A feasible solution trajectory.
   */
  std::unique_ptr<Solution> get_solution() {
    return solution_pool.get_best_solution();
  }

  /**
   * Run the Branch and Bound algorithm.
   * @param timelimit_s The timelimit in seconds, after which it aborts.
   * @param gap Allowed optimality gap.
   * @param verbose Defines if you want to see a progress log.
   */
  void optimize(int timelimit_s, double gap = 0.01, bool verbose = true) {
    print_start_stats(verbose);
    utils::Timer timer(timelimit_s);
    while (search_strategy.has_next()) {
      auto next = search_strategy.next();
      visit_node(next, gap);
      auto lb = get_lower_bound();
      auto ub = get_upper_bound();
      print_iteration_stats(verbose, lb, ub, timer.seconds());
      if (ub <= (1 + gap) * lb) { // check termination criterion
        break;
      }

      if (timer.timeout()) {
        print_timeout(verbose);
        break;
      }
    }
    print_final_stats(verbose);
  }

  std::unordered_map<std::string, std::string> get_statistics() const {
    std::unordered_map<std::string, std::string> stats;
    stats["num_iterations"] = std::to_string(num_iterations);
    stats["num_branches"] = std::to_string(num_branches);
    stats["num_explored"] = std::to_string(num_explored);
    return stats;
  }

private:
  void print_timeout(bool verbose) const {
    if (verbose) {
      std::cout << "Timeout." << std::endl;
    }
  }

  void print_start_stats(bool verbose) {
    if (verbose) {
      std::cout << "Starting with root node of size "
                << root->get_fixed_sequence().size() << std::endl;
      std::cout << "i\tLB\t|\tUB\t|\tTime" << std::endl;
    }
  }

  void print_iteration_stats(bool verbose, double lb, double ub,
                             double time_used) const {
    if (verbose) {
      if (num_iterations <= 10 ||
          (num_iterations < 100 && num_iterations % 10 == 0) ||
          (num_iterations < 1000 && num_iterations % 100 == 0) ||
          (num_iterations % 1000 == 0)) {
        std::cout << num_iterations << "\t" << lb << "\t|\t" << ub << "\t|\t"
                  << time_used << "s" << std::endl;
      }
    }
  }

  void print_final_stats(bool verbose) {
    if (verbose) {
      auto lb = get_lower_bound();
      auto ub = get_upper_bound();
      std::cout << "---------------" << std::endl
                << num_iterations << "\t" << lb << "\t|\t" << ub << std::endl;
      std::cout << num_iterations << " iterations with " << num_explored
                << " nodes explored and " << num_branches << " branches."
                << std::endl;
    }
  }

  /**
   * Check if the node's potential solution is above the upper  bound (or
   * close to it according  to  the gap) and prune it if  it is, as we won't
   * find a  solution that is better than the gap in this branch.
   * @param node The node to be checked
   * @param gap The gap. E.g. 0.01 means 1% within the current upper bound.
   * @return True iff the node was pruned.
   */
  bool prune_if_above_ub(std::shared_ptr<Node> &node, const double gap) {
    if (node->is_pruned() ||
        node->get_lower_bound() >=
            (1.0 - gap) * solution_pool.get_upper_bound()) {
      node->prune(false);
      on_prune(*node);
      return true;
    }
    return false;
  }

  /**
   * Executes a step/node exploration in the BnB-algorithm.
   * @return
   */
  void visit_node(std::shared_ptr<Node> &node, double gap) {
    ++num_iterations;
    // Automatically prune if worse than upper bound.
    if (prune_if_above_ub(node, gap)) {
      return;
    }
    // Explore  node.
    num_explored += 1;
    EventContext context{node, root, instance, &solution_pool, num_iterations};
    for (auto &callback : node_callbacks) {
      callback->on_entering_node(context);
    }
    if (!node->is_pruned()) { // the user callback may have pruned the node
      explore_node(node, context, gap);
    }
    for (auto &callback : node_callbacks) {
      callback->on_leaving_node(context);
    }
  }

  /**
   * Take a deeper look at the node.
   * @param node
   * @param context
   * @param gap
   */
  void explore_node(std::shared_ptr<Node> &node, EventContext &context,
                    const double gap) {
    add_lazy_constraints_if_feasible(node, context);
    if (node->is_feasible()) {
      process_feasible_node(node, context);
    } else {
      // Check again for the bound before branching.
      if (!prune_if_above_ub(node, gap)) {
        branch_node(node); // branch if not yet feasible.
      }
    }
  }

  void add_lazy_constraints_if_feasible(std::shared_ptr<Node> &node,
                                        EventContext &context) {
    if (node->is_feasible()) {
      // If node is feasible, check lazy constraints (the user may decide
      // to add further circles, making it infeasible again).
      for (auto &callback : node_callbacks) {
        callback->add_lazy_constraints(context);
        if (!node->is_feasible()) {
          break; // Stop after the solution became infeasible to keep the
          // number of added constraints low. High risk of redundant
          // constraints.
        }
      }
    }
  }

  void branch_node(std::shared_ptr<Node> &node) {
    if (branching_strategy.branch(*node)) {
      num_branches += 1;
      search_strategy.notify_of_branch(*node);
    }
  }

  void on_prune(Node &node) { search_strategy.notify_of_prune(node); }

  void process_feasible_node(std::shared_ptr<Node> &node,
                             EventContext &context) {
    solution_pool.add_solution(node->get_relaxed_solution());
    search_strategy.notify_of_feasible(*(context.current_node));
  }

  Instance *instance;              // the instance to solve.
  std::shared_ptr<Node> root;      // the root node to start the search with
  SearchStrategy &search_strategy; // will decide  which node to visit next
  std::vector<std::unique_ptr<B2BNodeCallback>>
      node_callbacks;                    // Allows to modify the BnB-behavior.
  BranchingStrategy &branching_strategy; // decides how to branch on a node, if
                                         // it is not yet feasible.
  SolutionPool solution_pool;            // Saves all solutions found so far.
  int num_iterations = 0;                // how many nodes have been looked at
  int num_explored = 0;                  // how many nodes have been explored
  int num_branches = 0; // how many of those nodes have been branched upon
};


} // namespace cetsp
#endif // CETSP_BNB_H
