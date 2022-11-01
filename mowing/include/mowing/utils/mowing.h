#ifndef TSPN_MOWING_UTILS_H
#define TSPN_MOWING_UTILS_H

#include <algorithm>
#include <vector>
#include <tuple>
#include "utils/utils.hpp"
#include "cgal.h"
#include "mowing/ExactOffsetCalculator.h"

#include <CGAL/Exact_circular_kernel_2.h>
#include <CGAL/Circular_kernel_intersections.h>
#include <CGAL/Circular_arc_2.h>
#include <CGAL/iterator.h>
#include <algorithm>

namespace mowing::utils {

        std::shared_ptr<Segment> minimum_distance(const Point &v, const Point &w, const Point &p);
        std::shared_ptr<Segment> minimum_distance(Point p, Point q, Point v, Point w);
        std::vector<Point> add_segment_to_tour(Polygon_2 &tour1, Polygon_2 &tour2, std::shared_ptr<Segment> &s,
                                               std::pair<Point, Point> &edge_to_split);

        std::shared_ptr<Segment> shortest_connecting_segment(Polygon_2 &polygon, Point &p);
        std::pair<std::shared_ptr<Segment>, std::shared_ptr<std::pair<Point, Point>>>
        shortest_connecting_segment(std::vector<Point> &tour,
                                    ExactOffsetCalculator::Polygon_with_holes_2 &region,
                                    bool isPath = false);

        std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>>
        shortest_connecting_segment_for_path(const std::vector<Point> &path, const Polygon_2 &poly2);

        std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>>
        shortest_connecting_segment(std::vector<Point> &point_list_1, std::vector<Point> &point_list_2);

        std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>>
        shortest_connecting_segment(const Polygon_2 &poly1, const Polygon_2 &poly2);
        std::vector<Point> shortest_segment_tour(std::vector<Point> &point_list_1, std::vector<Point> &point_list_2);

        void add_witnesses_around_point(Point p, Polygon_2 &polygon, double radius, std::vector<Point> &witnesses);
        std::vector<Point> get_points_along_edges(std::vector<Point> &tour, double distance);

        template<typename EdgeTypeIt>
        void get_points_along_edges(EdgeTypeIt begin, EdgeTypeIt end, double distance, std::vector<Point> &points);

        template<typename EdgeTypeIt>
        void get_points_along_half_edges(EdgeTypeIt begin, EdgeTypeIt end, double distance, std::vector<Point> &points);

        template<typename EdgeTypeIt>
        void
        get_points_along_half_edges_fixed_size(EdgeTypeIt begin, EdgeTypeIt end, std::size_t step_count, std::vector<Point> &points);

        Rational_Polygon_2 remove_close_points(Rational_Polygon_2 &polygon);
    }

#endif //TSPN_MOWING_UTILS_H
