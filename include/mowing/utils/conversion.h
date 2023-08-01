#ifndef LAWN_MOWING_CONVERSION_H
#define LAWN_MOWING_CONVERSION_H

#include "mowing/utils/definitions.h"

namespace mowing::utils {
    typedef mowing::definitions::ConicPolygon ConicPolygon;
    typedef mowing::definitions::InputPolygon InputPolygon;
    typedef mowing::definitions::PointVector PointVector;

    InputPolygon approximate_polygon(const ConicPolygon &P);
}

#endif //LAWN_MOWING_CONVERSION_H
