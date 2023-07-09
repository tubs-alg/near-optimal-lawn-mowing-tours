/**
 * This file computes the order on the convex hull.
 */
#ifndef CETSP_CONVEX_HULL_ORDER_H
#define CETSP_CONVEX_HULL_ORDER_H
#include "cetsp/common.h"
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/property_map.h>
namespace cetsp {
namespace details {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Segment_2 Segment_2;
typedef K::Ray_2 Ray_2;
typedef K::Direction_2 Direction_2;
typedef CGAL::Convex_hull_traits_adapter_2<
    K, CGAL::Pointer_property_map<Point_2>::type>
    Convex_hull_traits_2;

std::optional<double> get_distance_on_segment(const Segment_2 &s,
                                              const Point_2 &p);

class ConvexHullOrder {
  /**
   * This class computes a double value for all circles  intersecting
   * the convex hull that coincides with its position on the convex
   * hull.
   */
public:
  explicit ConvexHullOrder(const std::vector<Point> &points)
      : segments{compute_convex_hull_segments(points)} {}

  std::optional<double> operator()(const Circle &circle);

private:
  std::vector<Segment_2>
  compute_convex_hull_segments(const std::vector<Point> &points) const;

  std::vector<Segment_2> segments;
};

} // namespace details
} // namespace cetsp
#endif // CETSP_CONVEX_HULL_ORDER_H
