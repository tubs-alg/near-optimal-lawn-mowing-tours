#include "mowing/utils/mowing.h"

namespace mowing {
    namespace utils {

        std::shared_ptr<Segment> minimum_distance(const Point &v, const Point &w, const Point &p) {
            // Return minimum distance between line segment vw and point p
            auto l2 = CGAL::squared_distance(v, w);

            // Consider the line extending the segment, parameterized as v + t (w - v).
            // We find projection of point p onto the line.
            // It falls where t = [(p-v) . (w-v)] / |w-v|^2
            // We clamp t from [0,1] to handle points outside the segment vw.

            auto vec1 = Vector_2(v, p);
            auto vec2 = Vector_2(v, w);

            auto t = CGAL::max((Kernel::FT) 0.0, CGAL::min((Kernel::FT) 1.0, vec1 * vec2 / l2));
            Point projection = v + t * (w - v);  // Projection falls on the segment
            return std::make_shared<Segment>(p, projection);
        }

        std::shared_ptr<Segment> shortest_connecting_segment(Polygon_2 &polygon, Point &p) {

            std::shared_ptr<Segment> shortest_segment = nullptr;
            for (auto it = polygon.edges_begin(); it != polygon.edges_end(); it++) {

                auto min_dist_segment = minimum_distance(it->source(), it->target(), p);

                if (shortest_segment == nullptr ||
                    min_dist_segment->squared_length() < shortest_segment->squared_length()) {
                    shortest_segment = min_dist_segment;
                }
            }

            return shortest_segment;
        }

        std::shared_ptr<Segment> minimum_distance(Point p, Point q, Point v, Point w) {

            auto segments = std::vector<std::shared_ptr<Segment>>();

            if (p != q && p != v) segments.push_back(minimum_distance(p, q, v));
            if (p != q && p != w) segments.push_back(minimum_distance(p, q, w));

            if (v != w && v != p) segments.push_back(minimum_distance(v, w, p));
            if (v != w && v != q) segments.push_back(minimum_distance(v, w, q));

            std::shared_ptr<Segment> min_seg = *segments.begin();

            for (auto &s: segments) {
                if (s->squared_length() < min_seg->squared_length()) min_seg = s;
            }

            return min_seg;
        }

        std::vector<Point> add_segment_to_tour(Polygon_2 &tour1, Polygon_2 &tour2, std::shared_ptr<Segment> &s,
                                               std::pair<Point, Point> &edge_to_split) {
            // segment from point at tour1 to an edge of tour 2
            auto final_tour = std::vector<Point>();

            // Place the tour iterators at intersection
            auto tour1_circulator = tour1.vertices_circulator();
            while (*tour1_circulator.current_iterator() != s->source()) {
                tour1_circulator++;
            }

            auto tour2_circulator = tour2.vertices_circulator();

            if (tour2.size() > 2) {
                // Move circulator out of the way
                while (*tour2_circulator.current_iterator() == edge_to_split.first ||
                       *tour2_circulator.current_iterator() == edge_to_split.second) {
                    tour2_circulator++;
                }
            }

            while (*tour2_circulator.current_iterator() != edge_to_split.first &&
                   *tour2_circulator.current_iterator() != edge_to_split.second) {
                tour2_circulator++;
            }

            auto end_tour2 = tour2_circulator;
            auto end_tour1 = tour1_circulator;

            final_tour.push_back(*tour2_circulator.current_iterator());
            tour2_circulator++;

            final_tour.push_back(s->target());

            do {
                final_tour.push_back(*tour1_circulator.current_iterator());
                tour1_circulator++;
            } while (tour1_circulator != end_tour1);

            final_tour.push_back(s->source());
            final_tour.push_back(s->target());


            if (s->target() == *tour2_circulator.current_iterator()) {
                tour2_circulator++;
            } else {
                final_tour.push_back(*tour2_circulator.current_iterator());
                tour2_circulator++;
            }

            while (tour2_circulator != end_tour2) {
                final_tour.push_back(*tour2_circulator.current_iterator());
                tour2_circulator++;
            }

            auto last_val = final_tour[0];

            return final_tour;
        }

        std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>>
        shortest_connecting_segment(std::vector<Point> &point_list_1, std::vector<Point> &point_list_2) {

            Polygon_2 poly1(point_list_1.begin(), point_list_1.end());
            Polygon_2 poly2(point_list_2.begin(), point_list_2.end());

            return shortest_connecting_segment(poly1, poly2);
        }

        void shortest_segment_for_edges(const Point &source1, const Point &target1,
                                        const Point &source2, const Point &target2,
                                        std::shared_ptr<Segment> &min_seg,
                                        bool &segmentStartInPoly1,
                                        std::pair<Point, Point> &edge_to_split) {
            auto seg = minimum_distance(source1, target1, source2, target2);

            if (min_seg == nullptr || min_seg->squared_length() > seg->squared_length()) {
                min_seg = seg;
                segmentStartInPoly1 =
                        min_seg->source() == source1 || min_seg->source() == target1;

                if (segmentStartInPoly1) {
                    edge_to_split = std::make_pair(source2, target2);
                } else {
                    edge_to_split = std::make_pair(source1, target1);
                }
            }
        }


        std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>>
        shortest_connecting_segment(const Polygon_2 &poly1, const Polygon_2 &poly2) {
            std::shared_ptr<Segment> min_seg = nullptr;
            bool segmentStartInPoly1 = true;
            std::pair<Point, Point> edge_to_split = std::make_pair(Point(0, 0), Point(0, 0));

            for (auto it = poly1.edges_begin(); it != poly1.edges_end(); it++) {
                for (auto it2 = poly2.edges_begin(); it2 != poly2.edges_end(); it2++) {
                    shortest_segment_for_edges(it->source(),
                                               it->target(),
                                               it2->source(),
                                               it2->target(),
                                               min_seg,
                                               segmentStartInPoly1,
                                               edge_to_split);
                }
            }

            return std::make_tuple(min_seg, segmentStartInPoly1, edge_to_split);
        }

        std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>>
        shortest_connecting_segment_for_path(const std::vector<Point> &path, const Polygon_2 &poly2) {
            std::shared_ptr<Segment> min_seg = nullptr;
            bool segmentStartInPoly1 = true;
            std::pair<Point, Point> edge_to_split = std::make_pair(Point(0, 0), Point(0, 0));

            for (auto source = path.begin(); source != path.end()-1; source++) {
                auto target = source + 1;
                for (auto it = poly2.edges_begin(); it != poly2.edges_end(); it++) {
                    shortest_segment_for_edges(*source,
                                               *target,
                                               it->source(),
                                               it->target(),
                                               min_seg,
                                               segmentStartInPoly1,
                                               edge_to_split);
                }
            }

            return std::make_tuple(min_seg, segmentStartInPoly1, edge_to_split);
        }

        Polygon_2 conic_to_polygon_2(ExactOffsetCalculator::Polygon_with_holes_2 &poly) {
            Polygon_2 poly2;

            auto full_approximation = std::vector<std::pair<double, double>>();

            for (auto it = poly.outer_boundary().curves_begin(); it != poly.outer_boundary().curves_end(); it++) {
                auto approximation = std::vector<std::pair<double, double>>();
                it->polyline_approximation(3, std::back_inserter(approximation));

                if (!it->is_directed_right()) {
                    std::reverse(approximation.begin(), approximation.end());
                }

                approximation.pop_back();

                full_approximation.insert(full_approximation.end(), approximation.begin(), approximation.end());
            }

            for (auto &p: full_approximation) {
                poly2.push_back(Point(p.first, p.second));
            }

            return poly2;
        }

        std::pair<std::shared_ptr<Segment>, std::shared_ptr<std::pair<Point, Point>>>
        shortest_connecting_segment(std::vector<Point> &tour,
                                    ExactOffsetCalculator::Polygon_with_holes_2 &region,
                                    bool isPath) {
            std::tuple<std::shared_ptr<Segment>, bool, std::pair<Point, Point>> result;

            if(isPath) {
                result = shortest_connecting_segment_for_path(tour,
                                                              conic_to_polygon_2(region));
            } else {
                result = shortest_connecting_segment(Polygon_2(tour.begin(), tour.end()),

                                                     conic_to_polygon_2(region));
            }
            auto min_segment = std::get<0>(result);
            if (std::get<1>(result)) {
                return std::make_pair(min_segment, nullptr);
            }

            min_segment = std::make_shared<Segment>(min_segment->target(), min_segment->source());
            return std::make_pair(min_segment, std::make_shared<std::pair<Point, Point>>(std::get<2>(result)));
        }

        std::vector<Point>
        shortest_segment_tour(std::vector<Point> &point_list_1, std::vector<Point> &point_list_2) {
            auto segment_info = shortest_connecting_segment(point_list_1, point_list_2);
            std::shared_ptr<Segment> min_seg = std::get<0>(segment_info);
            bool segmentStartInPoly1 = std::get<1>(segment_info);
            std::pair<Point, Point> edge_to_split = std::get<2>(segment_info);

            Polygon_2 poly1(point_list_1.begin(), point_list_1.end());
            Polygon_2 poly2(point_list_2.begin(), point_list_2.end());

            if (segmentStartInPoly1) {
                return add_segment_to_tour(poly1, poly2, min_seg, edge_to_split);
            }

            return add_segment_to_tour(poly2, poly1, min_seg, edge_to_split);
        }

        bool longest(const std::vector<Point> &lhs, const std::vector<Point> &rhs) {
            return lhs.size() < rhs.size();
        }

        void add_witnesses_around_point(Point p, Polygon_2 &polygon, double radius, std::vector<Point> &witnesses) {
            typedef CGAL::Exact_circular_kernel_2 Circular_k;
            typedef CGAL::Point_2<Circular_k> Point_2;
            typedef CGAL::Circle_2<Circular_k> Circle_2;
            typedef CGAL::Circular_arc_2<Circular_k> Circular_arc_2;
            typedef CGAL::Line_2<Circular_k> Line_2;
            typedef CGAL::Line_arc_2<Circular_k> Line_arc_2;
            typedef std::pair<CGAL::Circular_arc_point_2
                    <Circular_k>, unsigned> PointPair_2;
            typedef typename CGAL::CK2_Intersection_traits<Circular_k, Circular_arc_2, Circular_arc_2>::type
                    Intersection_result;


            auto p_conv = Point_2(CGAL::to_double(p.x()), CGAL::to_double(p.y()));
            auto c = Circle_2(p_conv, radius);

            auto possibleFirstCircleOffsets = std::vector<std::pair<double,
                    double >>();
            possibleFirstCircleOffsets.emplace_back(radius, 0);
            possibleFirstCircleOffsets.emplace_back(-radius, 0);
            possibleFirstCircleOffsets.emplace_back(0, radius);
            possibleFirstCircleOffsets.emplace_back(0, -radius);

            auto possible_new_points = std::vector<std::vector<Point >>();
            for (auto &config: possibleFirstCircleOffsets) {
                auto c2 = Circle_2(Point_2(p_conv.x() + config.first, p_conv.y() + config.second), radius);

                auto intersections = std::vector<Intersection_result>();

                typedef CGAL::Dispatch_output_iterator<
                        std::tuple<PointPair_2, Circular_arc_2, Line_arc_2, Line_2>,
                        std::tuple<std::back_insert_iterator<std::vector<PointPair_2 >>,
                                std::back_insert_iterator<std::vector<Circular_arc_2 >>,
                                std::back_insert_iterator<std::vector<Line_arc_2 >>,
                                std::back_insert_iterator<std::vector<Line_2>>> > Dispatcher;

                auto point_pairs = std::vector<PointPair_2>();
                auto arcs = std::vector<Circular_arc_2>();
                auto line_arcs = std::vector<Line_arc_2>();
                auto lines = std::vector<Line_2>();

                Dispatcher disp = CGAL::dispatch_output<PointPair_2, Circular_arc_2, Line_arc_2, Line_2>(
                        std::back_inserter(point_pairs),
                        std::back_inserter(arcs),
                        std::back_inserter(line_arcs),
                        std::back_inserter(lines));

                CGAL::intersection(c, c2, disp);

                auto new_points = std::vector<Point>();

                auto first_point = Point(p.x() - config.first, p.y() - config.second);

                if (polygon.oriented_side(first_point) != CGAL::ON_POSITIVE_SIDE) {
                    new_points.emplace_back(first_point);
                }

                new_points.push_back(p);

                for (auto &point_pair : point_pairs) {
                    Point next_point = Point(CGAL::to_double(point_pair.first.x()),
                                             CGAL::to_double(point_pair.first.y()));

                    if (polygon.oriented_side(next_point) != CGAL::ON_POSITIVE_SIDE) {
                        new_points.emplace_back(next_point);
                    }
                }

                possible_new_points.push_back(new_points);
            }

            auto it = std::max_element(possible_new_points.begin(), possible_new_points.end(), longest);

            for (auto &point: *it) {
                witnesses.push_back(point);
            }
        }

        std::vector<Point> get_points_along_edges(std::vector<Point> &tour, double distance) {

            auto points = std::vector<Point>();

            for (auto it = tour.begin(); it != tour.end(); it++) {
                auto next = it + 1;
                if (next == tour.end()) next = tour.begin();

                auto edge_length = CGAL::squared_distance(*it, *next);
                double step_count = std::sqrt(CGAL::to_double(edge_length / (distance * distance)));
                for (double i = 1; i < step_count; i += 1) {
                    double fac1 = (i / step_count);
                    double fac2 = (step_count - i) / step_count;
                    points.emplace_back(it->x() * fac1 + next->x() * fac2,
                                        it->y() * fac1 + next->y() * fac2);
                }
            }

            return points;
        }

        template<typename EdgeTypeIt>
        void get_points_along_edges(EdgeTypeIt begin, EdgeTypeIt end, double distance, std::vector<Point> &points) {

            for (auto edge_it = begin; edge_it != end; edge_it++) {
                auto it = edge_it->source();
                auto next = edge_it->target();

                auto edge_length = CGAL::squared_distance(it, next);
                double step_count = std::sqrt(CGAL::to_double(edge_length / (distance * distance)));

                for (double i = 1; i < step_count; i += 1) {
                    double fac1 = (i / step_count);
                    double fac2 = (step_count - i) / step_count;
                    points.emplace_back(it.x() * fac1 + next.x() * fac2,
                                        it.y() * fac1 + next.y() * fac2);
                }
            }
        }

        template void get_points_along_edges(Polygon_2::Edge_const_iterator begin,
                                             Polygon_2::Edge_const_iterator end,
                                             double distance, std::vector<Point> &points);

        template void get_points_along_edges(CGAL::Polygon_2<Epick>::Edge_const_iterator begin,
                                             CGAL::Polygon_2<Epick>::Edge_const_iterator end,
                                             double distance, std::vector<Point> &points);

        template<typename EdgeTypeIt>
        void
        get_points_along_half_edges(EdgeTypeIt begin, EdgeTypeIt end, double distance, std::vector<Point> &points) {

            for (auto edge_it = begin; edge_it != end; edge_it++) {
                if (!edge_it->is_bisector()) {
                    continue;
                }

                auto it = edge_it->vertex()->point();
                auto next = edge_it->opposite()->vertex()->point();

                auto edge_length = CGAL::squared_distance(it, next);
                double step_count = std::sqrt(CGAL::to_double(edge_length / (distance * distance)));

                for (double i = 1; i < step_count; i += 1) {
                    double fac1 = (i / step_count);
                    double fac2 = (step_count - i) / step_count;
                    points.emplace_back(it.x() * fac1 + next.x() * fac2,
                                        it.y() * fac1 + next.y() * fac2);
                }
            }
        }

        template<typename EdgeTypeIt>
        void
        get_points_along_half_edges_fixed_size(EdgeTypeIt begin, EdgeTypeIt end, std::size_t step_count, std::vector<Point> &points) {

            for (auto edge_it = begin; edge_it != end; edge_it++) {
                if (!edge_it->is_bisector()) {
                    continue;
                }

                auto it = edge_it->vertex()->point();
                auto next = edge_it->opposite()->vertex()->point();

                //auto edge_length = CGAL::squared_distance(it, next);

                for (std::size_t i = 1; i < step_count; i += 1) {
                    double fac1 = ((double) i / (double) step_count);
                    double fac2 = (double) (step_count - i) / (double) step_count;
                    points.emplace_back(it.x() * fac1 + next.x() * fac2,
                                        it.y() * fac1 + next.y() * fac2);
                }
            }
        }

        template void get_points_along_half_edges_fixed_size(CGAL::Straight_skeleton_2<Epick>::Halfedge_iterator begin,
                                                  CGAL::Straight_skeleton_2<Epick>::Halfedge_iterator end,
                                                  std::size_t step_count, std::vector<Point> &points);

        template void get_points_along_half_edges(CGAL::Straight_skeleton_2<Epick>::Halfedge_iterator begin,
                                                  CGAL::Straight_skeleton_2<Epick>::Halfedge_iterator end,
                                                  double distance, std::vector<Point> &points);

        Rational_Polygon_2 remove_close_points(Rational_Polygon_2 &polygon) {

            if (polygon.size() <= 2) {
                return polygon;
            }

            Rational_Polygon_2 new_polygon;

            bool replacedPoints = false;

            for (auto it = polygon.vertices_begin(); it != polygon.vertices_end(); it++) {
                auto next = it + 1;
                if (next == polygon.vertices_end()) next = polygon.vertices_begin();

                if (CGAL::squared_distance(*it, *next) < 1e-4) {
                    auto midpoint = CGAL::midpoint(*it, *next);
                    new_polygon.push_back(*it);
                    replacedPoints = true;
                    it = next;
                } else {
                    new_polygon.push_back(*it);
                }
            }

            if (replacedPoints) {
                new_polygon = remove_close_points(new_polygon);
            }

            std::cout << "Removed points " << polygon.size() << " to " << new_polygon.size() << std::endl;

            return new_polygon;
        }

    }
}

