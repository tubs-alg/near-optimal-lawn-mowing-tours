#define BOOST_TEST_MODULE validation

#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include "utils/utils.hpp"
#include "mowing/LowerBoundSolver.h"
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

BOOST_AUTO_TEST_CASE(validate_solution)
{
    auto polygon = polygon_from_cgal_string("4 0 0 0 2 10 2 10 0");
    auto solver = mowing::LowerBoundSolver(polygon, mowing::definitions::INITIAL_STRATEGY_CH,
                                           mowing::definitions::FOLLOWUP_STRATEGY_GRID,
                                           1.0, 1800,
                                           4,
                                           50, 3);

    auto solution = solver.solve();

    /**
     * An optimal tour in that rectangle should have size 20-sqrt(3), i.e. two identical straight lines across
     * the polygon.
     */

    BOOST_TEST_MESSAGE("Checking the tour length");
    BOOST_CHECK_EQUAL(CGAL::to_double(CGAL::abs(polygon.area())), 20);
    BOOST_CHECK_LT(std::abs(solution.lower_bound - (20 - std::sqrt(3))), 1e-1);
}


BOOST_AUTO_TEST_CASE(validate_inequalities)
{
    auto polygon = polygon_from_cgal_string("10 -381.90187877705665 1231.2360605595322 -28.624271035705362 639.6967495591252 -441.6605872341724 530.7968302947075 -693.9383434236241 636.4499153229167 -693.1843529599948 -313.4563762612548 -669.5911368221488 42.604832447248995 -433.29678139608376 401.7494019893234 1039.4056917201779 -98.37613366782611 -302.54922785787306 1360.4052168234523 -448.323029414428 1443.4816722512535");
    auto solver = mowing::LowerBoundSolver(polygon, mowing::definitions::INITIAL_STRATEGY_CH,
                                           mowing::definitions::FOLLOWUP_STRATEGY_GRID,
                                           125.0, 1800,
                                           4,
                                           50, 3);
    auto solution = solver.solve();

    auto feasible_solution_solver = mowing::FeasibleToursFromLowerBound(solution, 1800, 20);
    auto second_solution = feasible_solution_solver.solve();

    BOOST_TEST_MESSAGE("Checking the inequalities");
    BOOST_CHECK_GE(second_solution.upper_bound, solution.lower_bound);
    BOOST_CHECK_GE(second_solution.lower_bound, solution.lower_bound);
}
