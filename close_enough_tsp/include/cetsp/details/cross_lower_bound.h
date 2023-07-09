/**
 * This file computes the order on the convex hull.
 */
#ifndef CROSS_LOWER_BOUND_H
#define CROSS_LOWER_BOUND_H

#include "cetsp/bnb.h"

namespace cetsp {
namespace details {

class CrossLowerBoundCallback : public B2BNodeCallback {
public:
  virtual ~CrossLowerBoundCallback() = default;
  virtual void on_entering_node(EventContext &context) {
    for (const auto &inter : context.current_node->get_intersections()) {

      /* Consider replacing an edge c1c2 (with exact points p1,p2) by the
       * workaround c1,q,c2 */
      auto calc_lowerbound_diff = [](const Circle &c1, const Circle &c2,
                                     const Point &p1, const Point &p2,
                                     const Circle &q) {
        double current_edge_len = p1.dist(p2);
        /* TODO use TripleMap */
        double workaround_len = compute_tour({c1, q, c2}, true).length();
        /* radiuses_compensation can be lower: the distance <p1, w> where w is
         * the point of p1 used by workaround_len */
        double radiuses_compensation = 2 * c1.radius + 2 * c2.radius;
        return -current_edge_len + workaround_len - radiuses_compensation;
      };

      /* For each edge (for example c1c2), consider removing it and go around
       * using c1,c3,c2 and similarly with c4 */
      std::vector<double> diffs{
          calc_lowerbound_diff(inter.c1, inter.c2, inter.p1, inter.p2,
                               inter.c3),
          calc_lowerbound_diff(inter.c1, inter.c2, inter.p1, inter.p2,
                               inter.c4),
          calc_lowerbound_diff(inter.c3, inter.c4, inter.p3, inter.p4,
                               inter.c1),
          calc_lowerbound_diff(inter.c3, inter.c4, inter.p3, inter.p4,
                               inter.c2),
      };
      double lower_bound_diff = *std::min_element(diffs.begin(), diffs.end());

      if (lower_bound_diff > 0) {
        double current_len = context.current_node->get_relaxed_solution()
                                 .get_trajectory()
                                 .length();
        context.current_node->add_lower_bound(current_len + lower_bound_diff);
      }
    }
  }
};

} // namespace details
} // namespace cetsp

#endif // CROSS_LOWER_BOUND_H
