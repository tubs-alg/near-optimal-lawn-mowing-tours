#ifndef LAWN_MOWING_STRAIGHT_SKELETON_PLACEMENT_STRATEGY_H
#define LAWN_MOWING_STRAIGHT_SKELETON_PLACEMENT_STRATEGY_H

#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"

namespace mowing {

    class StraightSkeletonPlacementStrategy : public WitnessPlacementStrategy {
    public:
        StraightSkeletonPlacementStrategy(InputPolygon &inputPolygon, double radius) :
        WitnessPlacementStrategy(inputPolygon, radius) {}
        StraightSkeletonPlacementStrategy() : WitnessPlacementStrategy() {}

        void placeWitnesses(PointVector &witnesses, const std::optional<std::size_t> &n, const std::optional<ConicPolygon> &region) override;
    private:
        void addInteriorWitnesses(std::vector<Point> &witnesses, CGAL::Polygon_2<Epick> &P, bool add_edges);
    };
}

#endif
