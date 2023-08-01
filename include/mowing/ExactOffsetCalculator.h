#ifndef CETSP_EPECK_OFFSET_CALCULATOR_H
#define CETSP_EPECK_OFFSET_CALCULATOR_H

#include <iostream>
#include <ostream>
#include <map>
#include <vector>
#include <iterator>
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_conic_traits_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/offset_polygon_2.h>
#include <CGAL/Polygon_set_2.h>

// Approximation stuff
#include <CGAL/approximated_offset_2.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
#include <CGAL/Polygon_convex_decomposition_2.h>


#include "mowing/utils/cetsp_tours.h"

namespace mowing {


    class ExactOffsetCalculator {

    public:
        typedef CGAL::Exact_predicates_exact_constructions_kernel InputKernel;
        typedef CGAL::Polygon_2<InputKernel> Input_Linear_polygon;
        typedef InputKernel::Point_2 Input_Point;

        typedef CGAL::CORE_algebraic_number_traits Nt_traits;
        typedef CGAL::Cartesian<Nt_traits::Rational> Kernel;
        typedef Kernel::Point_2 Point;

        typedef Nt_traits::Algebraic Algebraic;
        typedef CGAL::Cartesian<Algebraic> Alg_kernel;
        typedef CGAL::Arr_conic_traits_2<Kernel, Alg_kernel, Nt_traits> Traits;

        typedef CGAL::Polygon_2<Kernel> Linear_polygon;
        typedef CGAL::Gps_traits_2<Traits> Gps_traits;

        typedef CGAL::General_polygon_set_2<Gps_traits> Polygon_set_2;
        typedef Gps_traits::Polygon_2 Polygon_2;
        typedef Gps_traits::Polygon_with_holes_2 Polygon_with_holes_2;
        typedef Traits::X_monotone_curve_2 X_monotone_curve_2;

        typedef std::vector<Polygon_with_holes_2> ConicPolygonVector;

        ExactOffsetCalculator(Input_Linear_polygon &polygon, double radius, bool verbose = false);
        void initializeUncoveredRegions();
        void computeUncoveredRegions(std::vector<Input_Point> &points, bool interpret_as_path = false);
        Polygon_2 convert_input_polygon(Input_Linear_polygon &polygon);

        Polygon_set_2 polygon_set;
        Polygon_2 base_polygon;

    private:

        double radius;
        bool verbose;

        std::vector<Linear_polygon> extract_polygons(std::vector<Input_Point> &tour);
        std::vector<Polygon_with_holes_2> compute_covered_polygon(std::vector<Input_Point> &tour, bool interpret_as_path);

        Point convert_input_point(const Input_Point &p);

        bool do_inset_calculation(Linear_polygon &tour);
    };

}


#endif //CETSP_EPECK_OFFSET_CALCULATOR_H
