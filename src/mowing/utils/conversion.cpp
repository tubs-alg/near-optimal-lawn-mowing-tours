#include "mowing/utils/conversion.h"

namespace mowing::utils {

    InputPolygon approximate_polygon(const ConicPolygon &P) {
        std::size_t resolution = 0;

        InputPolygon polygon;
        std::size_t maxIterations = 50;
        std::size_t iteration = 0;

        do {
            resolution += 10;

            for (auto it = P.curves_begin(); it != P.curves_end(); it++) {
                auto approximation = std::vector<std::pair<double, double>>();

                it->polyline_approximation(resolution, std::back_inserter(approximation));
                if (!it->is_directed_right()) {
                    std::reverse(approximation.begin(), approximation.end());
                }

                approximation.pop_back();

                for (auto &pair: approximation) {
                    polygon.push_back(Point(pair.first, pair.second));
                }
            }
        } while (!polygon.is_simple() && iteration++ < maxIterations);

        return polygon;
    }
}