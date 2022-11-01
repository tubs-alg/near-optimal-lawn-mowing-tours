#include "mowing/witnesses/placement/initial/ConvexHullPlacementStrategy.h"

namespace mowing {

    void ConvexHullPlacementStrategy::placeWitnesses(PointVector &witnesses,
                                                     [[maybe_unused]]const std::optional<std::size_t> &n,
                                                     [[maybe_unused]]const std::optional<ConicPolygon> &region) {
        CGAL::ch_graham_andrew(inputPolygon.vertices_begin(),
                               inputPolygon.vertices_end(),
                               std::back_inserter(witnesses));
    }
}
