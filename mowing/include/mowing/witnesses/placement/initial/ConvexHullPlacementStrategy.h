#ifndef LAWN_MOWING_CONVEXHULLPLACEMENTSTRATEGY_H
#define LAWN_MOWING_CONVEXHULLPLACEMENTSTRATEGY_H

#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"
#include <CGAL/ch_graham_andrew.h>

namespace mowing {

    class ConvexHullPlacementStrategy : public WitnessPlacementStrategy {
    public:
        ConvexHullPlacementStrategy(InputPolygon &inputPolygon, double radius) :
        WitnessPlacementStrategy(inputPolygon, radius) {}
        ConvexHullPlacementStrategy() : WitnessPlacementStrategy() {}

        void placeWitnesses(PointVector &witnesses, const std::optional<std::size_t> &n = std::nullopt,
                            const std::optional<ConicPolygon> &region = std::nullopt) override;
    };
}

#endif //LAWN_MOWING_CONVEXHULLPLACEMENTSTRATEGY_H
