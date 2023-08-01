#ifndef LAWN_MOWING_KMEANS_SPARSIFICATION_H
#define LAWN_MOWING_KMEANS_SPARSIFICATION_H

#include "mowing/witnesses/sparsification/WitnessSparsification.h"
#include "mowing/utils/conversion.h"
#include "mowing/witnesses/placement/followup/RandomPlacementStrategy.h"

namespace mowing {

    class KMeansSparsification : public WitnessSparsification {
    public:
        std::optional<PointVector> sparsify(PointVector &witnesses,
                                            std::size_t n,
                                            const std::optional<ConicPolygon> &region,
                                            const std::optional<InputPolygon> &inputPolygon) override;

    };
}

#endif
