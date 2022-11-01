#ifndef TSPN_TOURS_H
#define TSPN_TOURS_H

#include "utils.hpp"
#include <vector>

namespace utils {

    inline
    Kernel::FT computePathLength(std::vector<Point> &path) {

        Kernel::FT pathLength = 0.0;

        for (auto it = path.begin(); it != path.end(); it++) {
            auto next = it + 1;
            if (next == path.end()) continue;

            pathLength += CGAL::approximate_sqrt(CGAL::squared_distance(*it, *next));
        }

        return pathLength;
    }

    inline
    Kernel::FT compute_tour_length(std::vector<Point> &tour) {
        Kernel::FT tourLength = computePathLength(tour);
        if(!tour.empty()) tourLength += CGAL::approximate_sqrt(CGAL::squared_distance(tour.back(), tour.front()));
        return tourLength;
    }

}

#endif //TSPN_TOURS_H
