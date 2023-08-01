#ifndef LAWN_MOWING_RANDOM_PLACEMENT_STRATEGY_H
#define LAWN_MOWING_RANDOM_PLACEMENT_STRATEGY_H

#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"
#include "utils/polygons.hpp"

namespace mowing {

    class RandomPlacementStrategy : public WitnessPlacementStrategy {
    public:
        RandomPlacementStrategy(InputPolygon &inputPolygon, double radius) :
        WitnessPlacementStrategy(inputPolygon, radius) {}
        RandomPlacementStrategy() : WitnessPlacementStrategy() {}

        void placeWitnesses(PointVector &witnesses, const std::optional<std::size_t> &n,
                            const std::optional<ConicPolygon> &region) override;
    };
}

#endif
