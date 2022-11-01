#include "mowing/ExactOffsetCalculator.h"

namespace mowing {

    ExactOffsetCalculator::ExactOffsetCalculator(Input_Linear_polygon &polygon, double radius, bool verbose) {

        this->radius = radius;
        this->base_polygon = this->convert_input_polygon(polygon);

        if (this->base_polygon.orientation() == CGAL::CLOCKWISE) {
            this->base_polygon.reverse_orientation();
        }

        this->initializeUncoveredRegions();

        this->verbose = verbose;
    }

    ExactOffsetCalculator::Polygon_2 ExactOffsetCalculator::convert_input_polygon(Input_Linear_polygon &polygon) {

        typedef Traits::Curve_2 Curve_2;
        typedef Traits::Rat_segment_2 Segment_2;
        typedef Gps_traits::Point_2 Conic_Point;

        if (polygon.is_clockwise_oriented()) {
            polygon.reverse_orientation();
        }

        Polygon_2 converted_polygon;

        for (auto it = polygon.edges_begin(); it != polygon.edges_end(); it++) {
            auto source = it->source();
            auto target = it->target();
            auto convertedSource = this->convert_input_point(source);
            auto convertedTarget = this->convert_input_point(target);

            auto circularArc = Curve_2(Segment_2(convertedSource, convertedTarget));

            auto xMonotoneConstructor = Traits::Make_x_monotone_2();
            auto xMonotoneCurves = std::vector<boost::variant<Conic_Point, X_monotone_curve_2>>();

            xMonotoneConstructor(circularArc, std::back_inserter(xMonotoneCurves));
            for (auto &val: xMonotoneCurves) {
                if (val.which() == 1) {
                    converted_polygon.push_back(boost::get<X_monotone_curve_2>(val));
                }
            }
        }

        return converted_polygon;
    }


    void ExactOffsetCalculator::computeUncoveredRegions(std::vector<Input_Point> &tour, bool interpret_as_path) {

        auto covered_polygons = this->compute_covered_polygon(tour, interpret_as_path);

        for (auto &covered_polygon: covered_polygons) {
            if (verbose) {
                std::cout << "Tour length " << tour.size() << std::endl;
                std::cout << "Covered Polygon has holes:" << covered_polygon.has_holes() << " polygon: "
                          << covered_polygon
                          << std::endl;
                std::cout.precision(20);
                std::cout << "Covered Polygon x: " << covered_polygon.outer_boundary().bbox().xmin() << "-"
                          << covered_polygon.outer_boundary().bbox().xmax()
                          << " y: " << covered_polygon.outer_boundary().bbox().ymin() << "-"
                          << covered_polygon.outer_boundary().bbox().ymax() << std::endl;

                std::cout << covered_polygon.is_plane() << " " << covered_polygon.is_unbounded() << std::endl;

                for (auto &hole: covered_polygon.holes()) {
                    std::cout << "HOLE " << hole << std::endl;
                }
            }

            this->polygon_set.difference(covered_polygon);

            if (verbose) {
                std::cout << "Difference operation successful" << std::endl;
            }
        }
    }


    void ExactOffsetCalculator::initializeUncoveredRegions() {
        this->polygon_set = Polygon_set_2();
        this->polygon_set.insert(this->base_polygon);
    }


    std::vector<ExactOffsetCalculator::Polygon_with_holes_2>
    ExactOffsetCalculator::compute_covered_polygon(std::vector<Input_Point> &points, bool interpret_as_path) {

        std::vector<Input_Point> tour(points.begin(), points.end());

        if (interpret_as_path) {
            auto extrude_point = [](const Input_Point &source, const Input_Point &target, const InputKernel::FT &c) {
                auto denominator = CGAL::approximate_sqrt(CGAL::square(target.x() - source.x()) +
                                                          CGAL::square(target.y() - source.y()));
                auto deltaX = (target.x() - source.x()) / denominator;
                auto deltaY = (target.y() - source.y()) / denominator;

                return Input_Point(target.x() + c * deltaX, target.y() + c * deltaY);
            };

            if (tour.front() == tour.back()) {
                tour.pop_back();
            } else {
                tour.insert(tour.begin(), extrude_point(*(tour.begin() + 1), *tour.begin(), 20));
                tour.insert(tour.end(), extrude_point(*(tour.end() - 2), tour.back(), 20));

            }
        }


        CGAL::Polygon_2<InputKernel> start_poly;
        start_poly.insert(start_poly.vertices_begin(), tour.begin(), tour.end());

        auto result = std::vector<Polygon_with_holes_2>();
        auto tour_polygons = this->extract_polygons(tour);

        for (auto &tour_polygon: tour_polygons) {

            /*std::ofstream f("polygon.txt");
            f.precision(20);
            f << tour_polygon;*/

            if (tour_polygon.is_simple()) {
                if (tour_polygon.is_clockwise_oriented()) {
                    tour_polygon.reverse_orientation();
                }

                Traits offsetTraits;
                Polygon_with_holes_2 covered_polygon = CGAL::offset_polygon_2(tour_polygon, Rat_kernel::FT(radius),
                                                                              offsetTraits);

                if (tour_polygon.area() > 0) {
                    // Holding the "inner" uncovered polygons
                    std::vector<Polygon_2> inset_polygons;
                   /* for (auto it = tour_polygon.vertices_begin(); it != tour_polygon.vertices_end(); it++) {
                        std::cout << Input_Point(it->x().doubleValue(), it->y().doubleValue()) << " - ";
                    }
                    std::cout << std::endl << "Tour polygon " << tour_polygon.is_simple() << " " << tour_polygon.area()
                              << std::endl;*/
                    CGAL::inset_polygon_2(tour_polygon, Rat_kernel::FT(radius), offsetTraits,
                                          std::back_inserter(inset_polygons));

                    for (auto &inset_polygon : inset_polygons) {
                        if (inset_polygon.orientation() == CGAL::COUNTERCLOCKWISE) {
                            inset_polygon.reverse_orientation();
                        }
                        covered_polygon.add_hole(inset_polygon);
                    }

                }

                result.push_back(covered_polygon);
            } else {
                if (verbose) {
                    std::cout << "START POLYGON " << start_poly << std::endl;
                    std::cout << "PARTS" << std::endl;
                    for (auto &t: tour_polygons) {
                        std::cout << t << std::endl;
                    }
                }
                throw std::runtime_error("Offset calculation yielded a polygon that is not simple.");
            }
        }

        return result;
    }

    ExactOffsetCalculator::Point ExactOffsetCalculator::convert_input_point(const Input_Point &p) {
        return Point(CGAL::to_double(p.x()), CGAL::to_double(p.y()));
    }

    std::vector<ExactOffsetCalculator::Linear_polygon>
    ExactOffsetCalculator::extract_polygons(std::vector<Input_Point> &tour) {
        typedef CGAL::Arr_segment_traits_2<InputKernel> Segment_traits_2;
        typedef CGAL::Arrangement_2<Segment_traits_2> Arrangement_2;
        typedef Arrangement_2::Face_handle Face;
        typedef InputKernel::Segment_2 Segment;

        auto segments = std::vector<Segment>();

        Segment_traits_2 geometricTraits;
        Arrangement_2 arr(&geometricTraits);

        for (auto it = tour.begin(); it != tour.end(); it++) {
            auto next = it + 1;
            if (next == tour.end()) next = tour.begin();

            if (*it != *next) segments.emplace_back(*it, *next);
        }

        CGAL::insert(arr, segments.begin(), segments.end());

        auto tour_polygons = std::vector<Linear_polygon>();

        auto add_antenna = [&tour_polygons, this](Arrangement_2::X_monotone_curve_2 &cv){
            auto p1 = this->convert_input_point(cv.left());
            auto p2 = this->convert_input_point(cv.right());

            if(p1 != p2) {
                Linear_polygon antenna;
                antenna.push_back(p1);
                antenna.push_back(p2);
                tour_polygons.push_back(antenna);
            }
        };

        for (auto it = arr.faces_begin(); it != arr.faces_end(); it++) {

            auto polygons = std::map<Face, std::vector<std::pair<Point,Point>>>();
            Segment_traits_2::X_monotone_curve_2 cv;
            if (it->has_outer_ccb()) {
                auto tour_polygon = std::vector<Input_Point>();
                auto cc = it->outer_ccb();
                do {
                    cv = cc->curve();
                    if (cc->face() == cc->twin()->face()) {
                        // Antenna
                        add_antenna(cv);
                    } else {
                        auto face_pair = cc->face();
                        if (polygons.find(face_pair) == polygons.end()) {
                            polygons[face_pair] = std::vector<std::pair<Point,Point>>();
                        }

                        polygons[face_pair].push_back(std::make_pair(this->convert_input_point(cv.left()),
                                                                     this->convert_input_point(cv.right())));
                    }
                } while (++cc != it->outer_ccb());
            }

            for (auto &val: polygons) {

                std::vector<Point> subtour;

                auto &edges = val.second;
                subtour.push_back(edges.back().first);
                edges.pop_back();

                bool foundSuccessor = true;

                while(foundSuccessor && !edges.empty()) {
                    foundSuccessor = false;
                    for(auto ptr = edges.begin(); ptr != edges.end(); ptr++) {
                        if(ptr->first == subtour.back() || ptr->second == subtour.back()) {
                            foundSuccessor = true;
                            subtour.push_back(ptr->first == subtour.back()? ptr->second : ptr->first);
                            edges.erase(ptr);
                            break;
                        }
                    }
                }

                mowing::utils::cleanup_tour(subtour);
                tour_polygons.emplace_back(subtour.begin(), subtour.end());

                std::cout << tour_polygons.back() <<std::endl;
            }
        }

        for (auto it = arr.halfedges_begin(); it != arr.halfedges_end(); it++) {
            if (it->face() == it->twin()->face()) {
                auto cv = it->curve();
                add_antenna(cv);
            }
        }

        if (tour_polygons.empty()) {
            std::cout << CGAL::Polygon_2<InputKernel>(tour.begin(), tour.end()) << std::endl;
            throw std::runtime_error("Strange tour occurance in exact offset calculation (no faces).");
        }

        // Remove duplicate tours (first trying consecutive duplicates as they are more likely)
        // auto unique_it = std::unique(tour_polygons.begin(), tour_polygons.end(), mowing::utils::equal<Linear_polygon>);
        // tour_polygons.resize(std::distance(tour_polygons.begin(), unique_it));
        // Now remove all remaining duplicates with n^2 runtime
        for (auto it = tour_polygons.begin(); it != tour_polygons.end();) {

            if(it->size() == 0) {
                it = tour_polygons.erase(it);
                continue;
            }

            bool hasDuplicate = false;
            for (auto it2 = std::next(it); it2 != tour_polygons.end(); it2++) {
                if (mowing::utils::equal(*it, *it2)) {
                    hasDuplicate = true;
                    break;
                }
            }

            if (hasDuplicate) {
                it = tour_polygons.erase(it);
            } else {
                it++;
            }
        }

        for (auto &poly: tour_polygons) {
            if (!poly.is_simple()) {
                std::cout << "Non simple " << poly << std::endl;
            }
            CGAL_assertion(poly.is_simple());
        }

        return tour_polygons;
    }

    bool ExactOffsetCalculator::do_inset_calculation(Linear_polygon &tour) {
        typedef CGAL::Gps_circle_segment_traits_2<InputKernel> Approximation_Traits;
        typedef Approximation_Traits::Polygon_2 Approximation_Polygon_2;
        typedef CGAL::Polygon_2<InputKernel> Approximation_Linear_polygon;

        Approximation_Linear_polygon P;

        for (auto it = tour.vertices_begin(); it != tour.vertices_end(); it++) {
            P.push_back(Input_Point(CGAL::to_double(it->x()), CGAL::to_double(it->y())));
        }

        std::vector<Approximation_Polygon_2> inset_polygons;
        CGAL::approximated_inset_2(P, this->radius, 1e-6, std::back_inserter(inset_polygons));

        return !inset_polygons.empty();
    }

}