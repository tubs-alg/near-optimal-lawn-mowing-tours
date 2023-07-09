/**
 * In this file we trigger_lazy_computation the convex hull order  that has to
 * be maintained. This order is unique, if every circle has only one closest
 * segment or vertex. Despite of that, it may suffer from small numerical
 * inaccuracies.
 *
 * If the convex  hull is degenerated to a line or a point, things become ugly.
 */
#include "cetsp/details/convex_hull_order.h"
namespace cetsp::details {

std::optional<double> get_distance_on_segment(const Segment_2 &s,
                                              const Point_2 &p) {
  // segment in range
  Ray_2 r1{s.source(), Direction_2{-(s.target().y() - s.source().y()),
                                   (s.target().x() - s.source().x())}};
  Ray_2 r2{s.target(), Direction_2{-(s.target().y() - s.source().y()),
                                   (s.target().x() - s.source().x())}};
  const auto dist_1 = squared_distance(r1, p);
  const auto dist_2 = squared_distance(r2, p);
  if (dist_1 <= s.squared_length() && dist_2 <= s.squared_length()) {
    // segment between both rays
    return {std::sqrt(squared_distance(r1, p))}; // add distance to first ray
  }
  return {};
}

std::optional<double> ConvexHullOrder::operator()(const Circle &circle) {
  /**
   * Computing the intersection point/distance on the convex hull for
   * ordering.
   */
  const Point_2 p{circle.center.x, circle.center.y};
  const double radius = circle.radius;
  double weight = 0.0;
  // find the closest segment
  auto closest_segment = std::min_element(
      segments.begin(), segments.end(), [&p](const auto s_a, const auto s_b) {
        return squared_distance(s_a, p) < squared_distance(s_b, p);
      });
  // if the closest segment is still out of range -> not ordered by CH.
  if (squared_distance(*closest_segment, p) > radius * radius) {
    return {}; // not on convex  hull;
  }
  // sum up  the lengths of all prior segments
  for (auto it = segments.begin(); it != closest_segment; ++it) {
    weight += std::sqrt(it->squared_length());
  }
  // plus the distance traveled on the closest.
  Ray_2 r1{
      closest_segment->source(),
      Direction_2{
          -(closest_segment->target().y() - closest_segment->source().y()),
          (closest_segment->target().x() - closest_segment->source().x())}};
  return weight + std::sqrt(squared_distance(r1, p));
}

std::vector<Segment_2> ConvexHullOrder::compute_convex_hull_segments(
    const std::vector<Point> &points) const {
  /**
   * Compute the segments on the convex hull, ordered counter-clockwise.
   */
  std::vector<Point_2> points_;
  points_.reserve(points.size());
  for (const auto &p : points) {
    points_.emplace_back(p.x, p.y);
  }
  std::vector<int> indices(points_.size()), out;
  std::iota(indices.begin(), indices.end(), 0);
  CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                      Convex_hull_traits_2(CGAL::make_property_map(points_)));
  std::vector<Segment_2> ch_segments;
  for (unsigned i = 0; i < out.size(); ++i) {
    ch_segments.emplace_back(points_[out[i]],
                             points_[out[(i + 1) % out.size()]]);
  }
  return ch_segments;
}
} // namespace cetsp::details
