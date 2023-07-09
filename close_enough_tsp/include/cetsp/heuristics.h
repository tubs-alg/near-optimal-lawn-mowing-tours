//
// Created by Dominik Krupke on 11.12.22.
//

#ifndef CETSP_HEURISTICS_H
#define CETSP_HEURISTICS_H
#include "cetsp/common.h"
namespace cetsp {
/**
 * Compute a heuristic solution using a procedure based on 2-Opt.
 * For this, only the circle's centers are considered, which
 * can lead to quite suboptimal solutions in some cases.
 */
auto compute_tour_by_2opt(Instance &instance) -> Solution;

} // namespace cetsp
#endif // CETSP_HEURISTICS_H
