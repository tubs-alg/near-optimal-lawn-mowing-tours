#include "mowing/witnesses/sparsification/KMeansSparsification.h"

namespace mowing {

    std::optional<WitnessSparsification::PointVector>
    KMeansSparsification::sparsify(PointVector &witnesses, std::size_t n,
                                   [[maybe_unused]]const std::optional<ConicPolygon> &region,
                                   [[maybe_unused]]const std::optional<InputPolygon> &inputPolygon) {

        if (witnesses.size() <= n) {
            return std::nullopt;
        }

        std::vector<Point> centroids;

        if (n == 0) {
            witnesses.clear();
            return centroids;
        }

        std::vector<std::size_t> labels(witnesses.size(), 0);

        RandomPlacementStrategy randomPlacementStrategy;
        randomPlacementStrategy.placeWitnesses(centroids, n, *region);

        assert(centroids.size() == n);

        std::vector<Point> oldCentroids;
        std::size_t currentIteration = 0;

        auto shouldStop = [&currentIteration, &centroids, &oldCentroids]() {
            bool stop = true;

            if (currentIteration > 200) return true;

            for (std::size_t i = 0; i < centroids.size(); i++) {
                if (centroids[i] != oldCentroids[i]) {
                    stop = false;
                    break;
                }
            }

            return stop;
        };

        auto getPointsForCentroid = [&witnesses, &labels](std::size_t k) {
            std::vector<Point> pointsForCentroid;
            for (std::size_t i = 0; i < labels.size(); i++) {
                if (labels[i] == k) {
                    pointsForCentroid.emplace_back(witnesses[i]);
                }
            }
            return pointsForCentroid;
        };

        auto calculateLabels = [&centroids, &witnesses, &labels]() {
            for (std::size_t i = 0; i < witnesses.size(); i++) {
                auto minDistance = CGAL::squared_distance(witnesses[i], centroids[0]);
                std::size_t label = 0;

                for (std::size_t k = 1; k < centroids.size(); k++) {
                    auto currentDistance = CGAL::squared_distance(witnesses[i], centroids[k]);
                    if (currentDistance < minDistance) {
                        minDistance = currentDistance;
                        label = k;
                    }
                }

                labels[i] = label;
            }
        };

        auto generateRandomPoint = [&region]() {
            std::vector<Point> centroidDummy; // Doing this to reuse the function
            RandomPlacementStrategy randomPlacementStrategy;
            randomPlacementStrategy.placeWitnesses(centroidDummy, 1, *region); // Adding 1 random centroids
            return *centroidDummy.begin();
        };

        auto calculateCentroids = [&centroids, &getPointsForCentroid, &generateRandomPoint]() {
            for (std::size_t k = 0; k < centroids.size(); k++) {
                std::vector<Point> pointsForCentroid = getPointsForCentroid(k);
                if (pointsForCentroid.empty()) {
                    centroids[k] = generateRandomPoint();
                } else {
                    centroids[k] = CGAL::centroid(pointsForCentroid.begin(), pointsForCentroid.end());
                }
            }
        };

        do {
            oldCentroids = std::vector<Point>(centroids);
            calculateLabels();
            calculateCentroids();
            currentIteration++;
        } while (!shouldStop());

        std::vector<Point> newWitnessSet;
        std::default_random_engine re(std::random_device{}());
        for (std::size_t k = 0; k < centroids.size(); k++) {
            std::vector<Point> pointsForCentroid = getPointsForCentroid(k);
            if (pointsForCentroid.empty()) {
                newWitnessSet.emplace_back(generateRandomPoint());
            } else {
                newWitnessSet.emplace_back(
                        *::utils::select_randomly(pointsForCentroid.begin(), pointsForCentroid.end(), re));
            }
        }

        witnesses.clear();
        witnesses.insert(witnesses.end(), newWitnessSet.begin(), newWitnessSet.end());

        return centroids;
    }
}
