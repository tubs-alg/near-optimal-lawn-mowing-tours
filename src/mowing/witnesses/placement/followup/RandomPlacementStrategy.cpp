#include "mowing/witnesses/placement//followup/RandomPlacementStrategy.h"

namespace mowing {
    void RandomPlacementStrategy::placeWitnesses(PointVector &witnesses, const std::optional<std::size_t> &n,
                                                 const std::optional<ConicPolygon> &region) {
        if (!n) {
            throw WitnessPlacementStrategy::WitnessPlacementException("Random strategy requires n");
        }

        if (!region) {
            throw WitnessPlacementStrategy::WitnessPlacementException("Random strategy requires region");
        }
        Polygon_2 polygon = mowing::utils::approximate_polygon(*region);

        for (std::size_t i = 0; i < *n; i++) {
            witnesses.emplace_back(::utils::random_point_inside_polygon(polygon));
        }
    }
}

