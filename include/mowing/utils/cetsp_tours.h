#ifndef CETSP_TOUR_UTILS_H
#define CETSP_TOUR_UTILS_H

#include <algorithm>
#include <vector>
#include <tuple>
#include "utils/utils.hpp"
#include "utils/cgal.h"
#include <algorithm>

namespace mowing {
    namespace utils {


        template<typename PointType>
        void cleanup_tour(std::vector<PointType> &tour);

        template<typename Polygon>
        bool equal(Polygon &poly1, Polygon &poly2);
    }
}


#endif //CETSP_TOUR_UTILS_H
