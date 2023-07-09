/**
 * This file implements branching strategies for the branch and bound algorithm,
 * i.e., deciding how to split  the solution space. The primary decision here
 * is to decide for the circle to integrate next. However, we can also do
 * some filtering here and only create branches  that are promising.
 *
 * The simplest branching strategy is to use always the farthest circle to
 * the current relaxed solution.
 *
 * If you want to improve the behaviour, the easiest way is probably to add
 * a rule, as for example done in ChFarhestCircle. A rule defines which
 * sequences are promising without actually evaluating them. If you need
 * to argue on the actual trajectory, use a callback directly added to the
 * branch and bound algorithm.
 *
 * 2023, Dominik Krupke, Tel Aviv
 */
#ifndef CETSP_BRANCHING_STRATEGY_H
#define CETSP_BRANCHING_STRATEGY_H
#include "cetsp/common.h"
#include "cetsp/details/convex_hull_order.h"
#include "cetsp/details/solution_pool.h"
#include "cetsp/details/triple_map.h"
#include "cetsp/node.h"
#include "rule.h"
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/property_map.h>
#include <random>
#include <vector>
namespace cetsp {

/**
 * The branching strategy decides how to split the solution space further.
 */
class BranchingStrategy {
public:
  /**
   * Allows you to setup some things for the rule at the beginning
   * of the branch and bound algorithm. For example change the mode
   * for tours or paths.
   * @param instance The instance to be solved.
   * @param root The root of the branch and bound tree.
   * @param solution_pool A pool of all solutions that will be found during BnB.
   */
  virtual void setup(Instance *instance, std::shared_ptr<Node> &root,
                     SolutionPool *solution_pool) {}
  /**
   * Branches the solution space of a node
   * @param node The node to be branched.
   * @return True iff the node has children.
   */
  virtual bool branch(Node &node) = 0;
  virtual ~BranchingStrategy() = default;
};

/**
 * The idea of Circle Branching is to add an still uncovered circle to the
 * sequence. This should be the most sensible branching strategy for most
 * cases, but it is not the only one. For example in case of intersections,
 * we know that the intersection has to be resolved and we could branch on
 * all ways to add a circle to the two edges in the .intersection.
 *
 * This is an abstract class and you need to specify how to choose the circle.
 * The most sensible approach should be to add the circle most distanced to
 * the current trajectory.
 */
class CircleBranching : public BranchingStrategy {
public:
  explicit CircleBranching(bool simplify = false, size_t num_threads = 1)
      : simplify{simplify}, num_threads{num_threads} {
    if (simplify) {
      std::cout << "Using node simplification." << std::endl;
    }
    std::cout << "Exploring on " << num_threads << " threads" << std::endl;
  }

  void setup(Instance *instance_, std::shared_ptr<Node> &root,
             SolutionPool *solution_pool) override {
    instance = instance_;
    for (auto &rule : rules) {
      rule->setup(instance, root, solution_pool);
    }
  }

  void add_rule(std::unique_ptr<SequenceRule> &&rule) {
    rules.push_back(std::move(rule));
  }

  bool branch(Node &node) override;

protected:
  /**
   * Override this method to filter the branching in advance.
   * @param sequence Sequence to be checked for a potential branch.
   * @return True if branch should be created.
   */
  virtual bool is_sequence_ok(const std::vector<int> &sequence,
                              const Node &parent) {
    return std::all_of(rules.begin(), rules.end(),
                       [&sequence, &parent](auto &rule) {
                         return rule->is_ok(sequence, parent);
                       });
  }

  /**
   * Return the cirlce to branch on. This allows to easily create different
   * strategies.
   * @param node The node to  be  branched.
   * @return Index to the circle, or None if no option for branchinng.
   */
  virtual std::optional<int> get_branching_circle(Node &node) = 0;

  Instance *instance = nullptr;
  bool simplify;
  size_t num_threads;
  std::vector<std::unique_ptr<SequenceRule>> rules;
};

/**
 * This strategy tries to branch on the circle that is most distanced
 * to the relaxed solution.
 */
class FarthestCircle : public CircleBranching {
public:
  explicit FarthestCircle(bool simplify = false, size_t num_threads = 1)
      : CircleBranching{simplify, num_threads} {
    std::cout << "Branching on farthest circle." << std::endl;
  }

protected:
  std::optional<int> get_branching_circle(Node &node) override;
};

/**
 * This strategy will only create branches that satisfy the CCW order of the
 * convex hull. A dependency is that the root is also obeying this rule.
 * We can proof that any optimal solution has follow the order of circles
 * intersecting the convex hull on the circle centers.
 *
 * This does not allow lazy constraints! (or only those that do not change
 * the convex hull).
 */
class ChFarthestCircle : public FarthestCircle {

public:
  explicit ChFarthestCircle(bool simplify = true, size_t num_threads = 1);
};

/**
 * Just a random branching  strategy as comparison. It will branch on a random
 * not yet covered circle.
 */
class RandomCircle : public CircleBranching {

public:
  explicit RandomCircle(bool simplify = false, size_t num_threads = 1)
      : CircleBranching{simplify, num_threads} {
    std::cout << "Branching on random circle" << std::endl;
  }

protected:
  std::optional<int> get_branching_circle(Node &node) override;
};

} // namespace cetsp
#endif // CETSP_BRANCHING_STRATEGY_H
