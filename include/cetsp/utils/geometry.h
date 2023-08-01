//
// Created by Dominik Krupke on 15.01.23.
//

#ifndef CETSP_GEOMETRY_H
#define CETSP_GEOMETRY_H
#include <cmath>
#include <utility>
namespace cetsp::utils {
double distance_to_segment(std::pair<double, double> s0,
                           std::pair<double, double> s1,
                           std::pair<double, double> p);

} // namespace cetsp::utils

#endif // CETSP_GEOMETRY_H
