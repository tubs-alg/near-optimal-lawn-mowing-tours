#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"

namespace mowing {
    WitnessPlacementStrategy::WitnessPlacementStrategy(InputPolygon &inputPolygon, double radius) :
            inputPolygon(inputPolygon),
            radius(radius) {}

    WitnessPlacementStrategy::WitnessPlacementStrategy() :
            inputPolygon(InputPolygon()),
            radius(0.0) {}

    void WitnessPlacementStrategy::cleanUpAndAddWitnesses(PointVector &points, PointVector &witnesses) {
        for (auto it = points.begin(); it != points.end();) {
            bool pointAlreadyPresent = false;

            for (auto it2 = it + 1; it2 != points.end(); it2++) {
                if (*it == *it2) {
                    pointAlreadyPresent = true;
                    break;
                }
            }

            for (auto &witness : witnesses) {
                if (*it == witness) {
                    pointAlreadyPresent = true;
                    break;
                }
            }

            if (pointAlreadyPresent) {
                it = points.erase(it);
            } else if (this->inputPolygon.has_on_unbounded_side(*it)) {
                auto segment = mowing::utils::shortest_connecting_segment(this->inputPolygon, *it);
                (*it) = segment->target();
                it++;
            } else {
                it++;
            }
        }

        for (auto &p: points) {
            witnesses.push_back(p);
        }
    }
}
