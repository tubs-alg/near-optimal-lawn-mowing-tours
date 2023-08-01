#include "mowing/witnesses/sparsification/DispersionSparsification.h"

namespace mowing {

    std::optional<WitnessSparsification::PointVector>
    DispersionSparsification::sparsify(PointVector &witnesses,
                                       std::size_t n,
                                       [[maybe_unused]]const std::optional<ConicPolygon> &region,
                                       [[maybe_unused]]const std::optional<InputPolygon> &inputPolygon) {

        if (witnesses.size() <= n) {
            return std::nullopt;
        }


        std::vector<double> distances_to_boundary;

        for (auto &witness: witnesses) {
            if (inputPolygon) {
                // Factor in the distance to the boundary (if needed) so that points are preferred that are farther away
                // from the boundary.
                std::vector<double> distances;
                for (auto it = inputPolygon->edges_begin(); it != inputPolygon->edges_end(); it++) {
                    auto s = ::utils::minimum_distance(it->source(), it->target(), witness);
                    distances.emplace_back(CGAL::to_double(s->squared_length()));
                }
                distances_to_boundary.emplace_back(*std::min_element(distances.begin(), distances.end()));
            } else {
                distances_to_boundary.emplace_back(0);
            }
        }

        using IndexPair = std::pair<std::size_t, std::size_t>;
        std::map<IndexPair, double> distances;

        std::set<size_t> selected_indices;
        std::vector<size_t> unselected_indices;

        double maxDistance = 0;
        double maxDistanceToBoundary = *std::max_element(distances_to_boundary.begin(), distances_to_boundary.end());

        for (std::size_t i = 0; i < witnesses.size(); i++) {
            unselected_indices.emplace_back(i);
            for (std::size_t j = i + 1; j < witnesses.size(); j++) {
                distances[std::make_pair(i, j)] = CGAL::to_double(CGAL::squared_distance(witnesses[i],
                                                                                         witnesses[j]));
                maxDistance = std::max(maxDistance, distances[std::make_pair(i, j)]);
            }
        }

        if (inputPolygon) {
            // Add boundary distance to distance matrix with a factor.
            for (std::size_t i = 0; i < witnesses.size(); i++) {
                for (std::size_t j = i + 1; j < witnesses.size(); j++) {
                    distances[std::make_pair(i, j)] = distances[std::make_pair(i, j)] / maxDistance +
                                                      +0.5 * (distances_to_boundary[i] / maxDistanceToBoundary +
                                                              distances_to_boundary[j] / maxDistanceToBoundary);
                }
            }
        }
        // Do 2-Approximation of p-dispersion
        auto longestDistancePair = std::max_element(distances.begin(), distances.end(),
                                                    [](const std::pair<IndexPair, double> &p1,
                                                       const std::pair<IndexPair, double> &p2) {
                                                        return p1.second < p2.second;
                                                    });

        auto delete_from_unselected = [&unselected_indices](const std::size_t &idx) {
            auto position = std::find(unselected_indices.begin(), unselected_indices.end(), idx);
            if (position != unselected_indices.end())
                unselected_indices.erase(position);
        };

        selected_indices.insert(longestDistancePair->first.first);
        selected_indices.insert(longestDistancePair->first.second);

        delete_from_unselected(longestDistancePair->first.first);
        delete_from_unselected(longestDistancePair->first.second);


        while (selected_indices.size() < n) {
            double overall_max_min_distance = 0;
            auto index_to_add = unselected_indices.front();

            for (auto &i: unselected_indices) {
                double min_distance = longestDistancePair->second;
                for (auto &j: selected_indices) {
                    min_distance = std::min(min_distance, distances[std::make_pair(std::min(i, j), std::max(i, j))]);
                }

                if (overall_max_min_distance < min_distance) {
                    index_to_add = i;
                    overall_max_min_distance = min_distance;
                }
            }

            selected_indices.insert(index_to_add);
            delete_from_unselected(index_to_add);
        }

        std::sort(unselected_indices.begin(), unselected_indices.end());  // Make sure the container is sorted
        for (auto it = unselected_indices.rbegin(); it != unselected_indices.rend(); it++) {
            witnesses.erase(witnesses.begin() + (long) *it);
        }

        return std::nullopt;
    }
}

