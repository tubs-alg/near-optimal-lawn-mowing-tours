#ifndef LAWN_MOWING_GRID_PLACEMENT_STRATEGY_H
#define LAWN_MOWING_GRID_PLACEMENT_STRATEGY_H

#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"

namespace mowing {

    class GridPlacementStrategy : public WitnessPlacementStrategy {
    public:
        GridPlacementStrategy(InputPolygon &inputPolygon, double radius) :
        WitnessPlacementStrategy(inputPolygon, radius) {}
        GridPlacementStrategy() : WitnessPlacementStrategy() {}

        void placeWitnesses(PointVector &witnesses, const std::optional<std::size_t> &n,
                            const std::optional<ConicPolygon> &region) override;
    };
}

#endif
