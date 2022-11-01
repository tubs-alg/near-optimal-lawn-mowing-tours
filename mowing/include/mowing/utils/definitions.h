#ifndef LAWN_MOWING_DEFINITIONS_H
#define LAWN_MOWING_DEFINITIONS_H

#include <vector>
#include "mowing/ExactOffsetCalculator.h"

namespace mowing::definitions {
    static const int INITIAL_STRATEGY_CH = 1;
    static const int INITIAL_STRATEGY_VERTICES = 2;

    static const int FOLLOWUP_STRATEGY_RANDOM = 3;
    static const int FOLLOWUP_STRATEGY_GRID = 4;
    static const int FOLLOWUP_STRATEGY_SKELETON = 5;

    typedef std::vector<Point> PointVector;
    typedef ExactOffsetCalculator::Gps_traits::General_polygon_2 ConicPolygon;
    typedef ExactOffsetCalculator::ConicPolygonVector ConicPolygonVector;
    typedef ExactOffsetCalculator::Polygon_with_holes_2 Conic_Polygon_with_holes_2;
    typedef Polygon_2 InputPolygon;


    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds TimeInterval;
}


#endif //LAWN_MOWING_DEFINITIONS_H
