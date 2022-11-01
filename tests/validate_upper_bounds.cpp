#define BOOST_TEST_MODULE validation

#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include "utils/utils.hpp"
#include "mowing/MowingSolverWithUpperBound.h"
#include "mowing/FeasibleToursFromLowerBound.h"
#include "mowing/utils/definitions.h"
#include "utils/utils.hpp"

using namespace boost::unit_test;

Polygon_2 polygon_from_cgal_string(const std::string &s) {
    Polygon_2 polygon;
    std::stringstream ss;
    ss << s;
    ss >> polygon;
    return polygon;
}

BOOST_AUTO_TEST_CASE(validate_solution_ch_grid)
{
    auto polygon = polygon_from_cgal_string("4 0 0 0 2 10 2 10 0");
    auto solver = mowing::LowerBoundSolver(polygon, mowing::definitions::INITIAL_STRATEGY_CH,
                                           mowing::definitions::FOLLOWUP_STRATEGY_GRID,
                                           1.0, 1800,
                                           4,
                                           50, 3);
    auto solution = solver.solve();

    auto feasible_solution_solver = mowing::FeasibleToursFromLowerBound(solution, 1800, 20);
    auto second_solution = feasible_solution_solver.solve();


    /**
     * An optimal tour in that rectangle should have size 20-sqrt(3), i.e. two identical straight lines across
     * the polygon.
     */

    BOOST_TEST_MESSAGE("Checking the tour length");
    BOOST_CHECK_EQUAL(CGAL::to_double(CGAL::abs(polygon.area())), 20);
    BOOST_CHECK_LT(std::abs(solution.lower_bound - (20 - std::sqrt(3))), 1e-1);
    BOOST_CHECK_LT(std::abs(second_solution.lower_bound - (20 - std::sqrt(3))), 1e-1);
    BOOST_CHECK_LT(std::abs(second_solution.upper_bound - (20 - std::sqrt(3))), 1e-1);
}
