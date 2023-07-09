//
// Created by Dominik Krupke on 19.01.23.
//

#ifndef CETSP_GLOBAL_CONVEX_HULL_RULE_H
#define CETSP_GLOBAL_CONVEX_HULL_RULE_H
#include "cetsp/common.h"
#include "cetsp/details/solution_pool.h"
#include "cetsp/node.h"
#include "cetsp/strategies/rule.h"
namespace cetsp {

class GlobalConvexHullRule : public SequenceRule {
public:
  virtual void setup(const Instance *instance, std::shared_ptr<Node> &root,
                     SolutionPool *solution_pool);

  static bool
  is_path_sequence_possible(const std::vector<int> &sequence, unsigned int n,
                            const std::vector<bool> &is_in_ch,
                            const std::vector<double> &order_values);
  virtual bool is_ok(const std::vector<int> &seq, const Node &parent);

private:
  const Instance *instance = nullptr;
  std::vector<double> order_values;
  std::vector<bool> is_ordered;

  bool sequence_is_ch_ordered(const std::vector<int> &sequence);
  std::vector<Point> get_circle_centers(const Instance &instance) const;
  void compute_weights(const Instance *instance, std::shared_ptr<Node> &root);
};
} // namespace cetsp
#endif // CETSP_GLOBAL_CONVEX_HULL_RULE_H
