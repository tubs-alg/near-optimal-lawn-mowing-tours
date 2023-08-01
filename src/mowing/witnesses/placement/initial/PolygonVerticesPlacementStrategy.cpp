#include "mowing/witnesses/placement/initial/PolygonVerticesPlacementStrategy.h"

namespace mowing {

    void PolygonVerticesPlacementStrategy::placeWitnesses(PointVector &witnesses,
                                                          [[maybe_unused]]const std::optional<std::size_t> &n,
                                                          [[maybe_unused]]const std::optional<ConicPolygon> &region) {
        for (auto it = inputPolygon.vertices_begin(); it != inputPolygon.vertices_end(); it++) {
            witnesses.emplace_back(*it);
        }
    }
}
