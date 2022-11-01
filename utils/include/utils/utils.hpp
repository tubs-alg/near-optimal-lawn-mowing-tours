#ifndef TSPN_UTILS_HPP
#define TSPN_UTILS_HPP

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_triangulation_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_polygon_2.h>
#include <CGAL/ch_graham_andrew.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Circle_2.h>
#include <CGAL/Point_2.h>
#include <CGAL/Point_3.h>
#include <CGAL/Cartesian.h>
#include <CGAL/centroid.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/squared_distance_3.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <utility>


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Epick;
typedef Kernel::Point_2 Point;
typedef Kernel::Point_3 Point_3;
// typedef Kernel::Triangle_2 Triangle;
typedef Kernel::Line_2 Line;
typedef Kernel::Segment_2 Segment;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
typedef CGAL::Circle_2<Kernel> Circle;
typedef CGAL::Vector_2<Kernel> Vector_2;
typedef CGAL::Straight_skeleton_2<Kernel> Straight_skeleton;

inline std::istream &operator>>(std::istream &is, std::vector<Point> &out) {
    int n = 0; // number of vertices
    is >> n;
    Point point;
    if (is) {
        out.clear();
        for (int i = 0; i < n; i++) {
            if (is >> point) {
                out.push_back(point);
            } else {
                return is;
            }
        }
    }
    return is;
}

inline std::ostream &operator<<(std::ostream &os, const std::vector<Point> &in) {
    os << in.size() << ' ';
    for (auto i = in.begin(); i != in.end(); ++i) {
        os << *i << ' ';
    }
    return os;
}

namespace utils {
    inline bool do_overlap(Polygon_2 &inner_polygon, Polygon_2 &outer_polygon) {
        for (auto vertex_it = inner_polygon.vertices_begin(); vertex_it != inner_polygon.vertices_end(); ++vertex_it) {
            if (outer_polygon.bounded_side(*vertex_it) == CGAL::ON_BOUNDED_SIDE) {
                return true;
            }
        }
        return false;
    }


    template <typename Ref>
    struct lvalue_or_rvalue {

        Ref &&ref;

        template <typename Arg>
        constexpr lvalue_or_rvalue(Arg &&arg) noexcept
                :   ref(std::move(arg))
        { }

        constexpr operator Ref& () const & noexcept { return ref; }
        constexpr operator Ref&& () const && noexcept { return std::move(ref); }
        constexpr Ref& operator*() const noexcept { return ref; }
        constexpr Ref* operator->() const noexcept { return &ref; }

    };
}
#endif