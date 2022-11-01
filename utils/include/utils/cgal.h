#ifndef CETSP_DEFINITIONS_H
#define CETSP_DEFINITIONS_H

#include "utils/utils.hpp"

#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_conic_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/centroid.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/squared_distance_3.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/offset_polygon_2.h>
#include <CGAL/Polygon_set_2.h>
#include <CGAL/Arr_circle_segment_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::CORE_algebraic_number_traits Nt_traits;
typedef CGAL::Cartesian<Nt_traits::Rational> Rat_kernel;
typedef Nt_traits::Algebraic Algebraic;
typedef CGAL::Cartesian<Algebraic> Alg_kernel;


typedef CGAL::Arr_conic_traits_2<Rat_kernel, Alg_kernel, Nt_traits>
        Conic_Traits;

typedef CGAL::Polygon_2<Rat_kernel> Rational_Polygon_2;
typedef CGAL::Gps_traits_2<Conic_Traits> Gps_traits;
typedef Gps_traits::Polygon_2 Conic_Polygon_2;
typedef Gps_traits::Polygon_with_holes_2 Conic_Polygon_with_holes_2;
typedef CGAL::General_polygon_set_2<Gps_traits> Conic_Polygon_set_2;
typedef Conic_Traits::X_monotone_curve_2 X_monotone_curve_2;
typedef Conic_Traits::Point_2 Conic_Point;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_polyline_traits_2.h>
#include <CGAL/Arrangement_2.h>

typedef CGAL::Arr_segment_traits_2<Kernel>                Segment_traits_2;
typedef CGAL::Arrangement_2<Segment_traits_2>             Arrangement_2;

#endif //CETSP_DEFINITIONS_H
