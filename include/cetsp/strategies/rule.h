/**
 * This file defines the base class for a rule, use in the BranchingStrategy.
 * It allows you to cheaply prevent the creation of obviously bad branches, if
 * you have some nice argument. Also take a look on callbacks. These rules
 * are actually applied before the creation of a branch and may be much cheaper
 * in some cases.
 *
 * 2023, Dominik Krupke, Tel Aviv
 */
#ifndef CETSP_RULE_H
#define CETSP_RULE_H
#include "../common.h"
#include "../details/solution_pool.h"
#include "../node.h"
namespace cetsp {
/**
 * A SequenceRule can be used to exclude branches only based on their
 * sequences, not on their trajectories. The most powerful rule, we now
 * so far of, is to check if the sequence obeys the order of the convex
 * hull, which is provably necessary for optimality.
 *
 * You can just inherit the rule and then add `add_rule` on the Branching
 * Strategy. If you need the actual trajectory, add a callback directly
 * to the branch and bound algorithm, as this allows automatic caching.
 */
class SequenceRule {
public:
  virtual void setup(const Instance *instance, std::shared_ptr<Node> &root,
                     SolutionPool *solution_pool) = 0;
  virtual bool is_ok(const std::vector<int> &seq, const Node &parent) = 0;
  virtual ~SequenceRule() = default;
};

} // namespace cetsp
#endif // CETSP_RULE_H
