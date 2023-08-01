#include "mowing/witnesses/placement/followup/GridPlacementStrategy.h"

namespace mowing {
    void GridPlacementStrategy::placeWitnesses(PointVector &witnesses,
                                               [[maybe_unused]]const std::optional<std::size_t> &n,
                                               [[maybe_unused]]const std::optional<ConicPolygon> &region) {

        if (!region) {
            throw WitnessPlacementStrategy::WitnessPlacementException("Grid strategy requires region");
        }

        Polygon_2 polygon = mowing::utils::approximate_polygon(*region);
        std::vector<Point> points = std::vector<Point>();

        auto bbox = region->bbox();

        double minX = bbox.xmin(), maxX = bbox.xmax(),
                minY = bbox.ymin(), maxY = bbox.ymax();

        for (double x = minX; x <= maxX; x += this->radius * 0.25) {
            for (double y = minY; y <= maxY; y += this->radius * 0.25) {
                auto p = Point(x, y);
                if (polygon.has_on_bounded_side(p) == CGAL::ON_BOUNDED_SIDE) {
                    points.emplace_back(p);
                }
            }
        }

        this->cleanUpAndAddWitnesses(points, witnesses);
    }
}

