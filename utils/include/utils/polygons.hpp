#ifndef TSPN_POLYGONS_H
#define TSPN_POLYGONS_H

#include "utils.hpp"
#include <random>
#include <algorithm>
#include <vector>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_2_algorithms.h>

typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;

namespace utils {

    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator &g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }

    /*std::vector<Polygon_2> compute_simple_intersection(Polygon_2 &poly1, Polygon_2 &poly2, std::size_t n = 1) {
        /// Computes an intersection between two polygons where the expected result yields n simple subpolygons

        std::vector<Polygon_with_holes_2> intersections;
        auto result = std::vector<Polygon_2>();

        CGAL::intersection(poly1, poly2, std::back_inserter(intersections));

        if (intersections.size() <= n) {
            for (auto &intersection: intersections) {
                if (intersection.has_holes()) {
                    throw std::runtime_error("Unexpected hole in presumably simple polygon");
                }

                result.push_back(intersection.outer_boundary());
            }
        }

        return result;
    }*/


    template<typename P>
    Point random_point_inside_polygon(const P &polygon) {
        auto bbox = polygon.bbox();

        bool is_simple = polygon.is_simple();

        std::uniform_real_distribution<double> xDistribution(bbox.xmin(), bbox.xmax());
        std::uniform_real_distribution<double> yDistribution(bbox.ymin(), bbox.ymax());
        std::default_random_engine re(std::random_device{}());
        Point p;
        for (std::size_t i = 0; i < 10000; i++) {
            p = Point(xDistribution(re), yDistribution(re));
            if (!is_simple) return p;
            if (polygon.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) return p;
        }

        auto gen = std::mt19937{std::random_device{}()};

        std::cout << "HAD TO USE A POLYGON ENDPOINT" << std::endl;
        return *select_randomly(polygon.vertices_begin(),
                                polygon.vertices_end(),
                                gen);
    }

}

#endif //TSPN_POLYGONS_H
