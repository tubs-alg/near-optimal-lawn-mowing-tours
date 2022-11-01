#ifndef LAWN_MOWING_POLYGON_VERTICES_PLACEMENT_STRATEGY_H
#define LAWN_MOWING_POLYGON_VERTICES_PLACEMENT_STRATEGY_H

#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"

namespace mowing {
    class PolygonVerticesPlacementStrategy : public WitnessPlacementStrategy {
    public:
        PolygonVerticesPlacementStrategy(InputPolygon &inputPolygon, double radius) :
                WitnessPlacementStrategy(inputPolygon, radius) {}
        PolygonVerticesPlacementStrategy() : WitnessPlacementStrategy() {}

        void placeWitnesses(PointVector &witnesses, const std::optional<std::size_t> &n,
                            const std::optional<ConicPolygon> &region) override;
    };
}
#endif
