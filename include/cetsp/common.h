//
// Provides some common classes on instances and solutions that are
// independent of the concrete algorithms.
//

#ifndef CLOSE_ENOUGH_TSP_COMMON_H
#define CLOSE_ENOUGH_TSP_COMMON_H
#include "details/cgal_kernel.h"
#include "utils/geometry.h"
#include <CGAL/squared_distance_2.h> //for 2D functions
#include <cmath>
#include <utility>

namespace cetsp {
class Point {
  /**
   * Represents a single coordinate.
   */
public:
  Point() {}
  Point(double x, double y) : x{x}, y{y} {}
  double x;
  double y;

  [[nodiscard]] double dist(const Point &point) const {
    return std::sqrt(squared_dist(point));
  }

  [[nodiscard]] double squared_dist(const Point &point) const {
    return (point.x - x) * (point.x - x) + (point.y - y) * (point.y - y);
  }

  bool operator==(const Point &point) const {
    return point.x == x && point.y == y;
  }
  bool operator!=(const Point &point) const { return !((*this) == point); }
};

class Circle {
  /**
   * Represents a circle, consisting of a center and a radius.
   */
public:
  Circle(){};
  Circle(Point center, double radius) : center{center}, radius{radius} {}

  bool contains(const Point &point) const {
    return center.squared_dist(point) <= radius * radius;
  }

  bool contains(const Circle &circle) const {
    return center.dist(circle.center) + circle.radius <= 1.001 * radius;
  }

  Point center;
  double radius;
};


class Instance : public std::vector<Circle> {
public:
  Instance() {}
  explicit Instance(std::vector<Circle> circles) {
    reserve(circles.size());
    std::sort(circles.begin(), circles.end(),
              [](const auto &a, const auto &b) { return a.radius < b.radius; });
    for (const auto &circle : circles) {
      if (std::any_of(begin(), end(),
                      [&circle](auto &c) { return circle.contains(c); })) {
        std::cout << "Removed implicit circle (" << circle.center.x << ", "
                  << circle.center.y << ")" << std::endl;
        continue;
      }
      push_back(circle);
    }
  }
  [[nodiscard]] bool is_path() const {
    if (path) {
      return true;
    } else {
      return false;
    }
  }

  [[nodiscard]] bool is_tour() const {
    if (path) {
      return false;
    }
    { return true; }
  }

  void add_circle(Circle &circle) {
    if (std::any_of(begin(), end(),
                    [&circle](auto &c) { return circle.contains(c); })) {
      return;
    }
    push_back(circle);
    revision += 1;
  }

  std::optional<std::pair<Point, Point>> path;
  int revision =
      0; // actually the size  should already say enough about  the revision.
  double eps = 0.01;
};

class Trajectory {
  /**
   * For representing the trajectory in a solution.
   */
public:
  Trajectory() = default;
  explicit Trajectory(std::vector<Point> points) : points{std::move(points)} {}

  bool is_tour() const { return points[0] == points[points.size() - 1]; }

  /**
   * Returns a sub-trajectory. If the trajectory is a tour, begin can be
   * after end (then modulo is used, the last point will not be repeated).
   * @param begin The index of the first point in the sub-trajectory.
   * @param end The index of the last point in the sub-trajectory.
   * @return The sub-trajectory.
   */
  Trajectory sub(int begin, int end) const {
    std::vector<Point> path;
    auto n = points.size() - 1;
    auto i = begin;
    if (is_tour()) {
      for (; i > end; i = (i + 1) % n) {
        path.push_back(points[i]);
      }
    }
    for (; i <= end; ++i) {
      path.push_back(points[i]);
    }
    return Trajectory{path};
  }

  double distance(const Circle &circle) const {
    double min_dist = std::numeric_limits<double>::infinity();
    if (points.size() == 1) {
      details::cgPoint tp(points[0].x, points[0].y);
      min_dist =
          points[0].dist(circle.center); // CGAL::squared_distance(tp, p);
    }
    for (unsigned i = 0; i < points.size() - 1; i++) {
      auto dist = utils::distance_to_segment(
          {points[i].x, points[i].y}, {points[i + 1].x, points[i + 1].y},
          {circle.center.x, circle.center.y});
      if (dist < min_dist) {
        min_dist = dist;
      }
    }
    return min_dist - circle.radius;
  }

  double length() const {
    if (!_length) {
      double l = 0;
      for (unsigned i = 0; i < points.size() - 1; i++) {
        details::cgSegment segment({points[i].x, points[i].y},
                                   {points[i + 1].x, points[i + 1].y});
        l += std::sqrt(segment.squared_length());
      }
      _length = l;
    }
    return *_length;
  }

  bool is_simple() const {
    if (points.front() == points.back()) {
      auto points_ = get_simplified_points(0.01);
      details::cgPolygon poly{points_.begin(), points_.end() - 1};
      return poly.is_simple();
    } else {
      std::cout << "Warning! `is_simple`  does not work for paths right now!"
                << std::endl;
      return false;
    }
  }

  [[nodiscard]] bool covers(const Circle &circle,
                            double FEASIBILITY_TOLERANCE = 0.0) const {
    return distance(circle) <= FEASIBILITY_TOLERANCE;
  }

  template <typename It>
  [[nodiscard]] auto covers(It begin, It end,
                            double FEASIBILITY_TOLERANCE = 0.0) const -> bool {
    return std::all_of(begin, end, [&](const Circle &c) {
      return this->covers(c, FEASIBILITY_TOLERANCE);
    });
  }

  std::vector<Point> points;

private:
  /**
   * Merge very close points to prevent numerical stuff (there are often
   * the same point with some tiny numerical variation.
   * @param eps Points with this  distance are considered identical.
   * @return
   */
  std::vector<details::cgPoint> get_simplified_points(double eps = 0.01) const {
    std::vector<details::cgPoint> points_;
    for (const auto &p : points) {
      if (!points_.empty() &&
          p.dist(Point(points_.back().x(), points_.back().y())) < eps) {
        continue;
      }
      points_.emplace_back(p.x, p.y);
    }
    return points_;
  }
  mutable std::optional<double> _length;
};
}; // namespace cetsp

#endif // CLOSE_ENOUGH_TSP_COMMON_H
