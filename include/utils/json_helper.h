#ifndef LAWN_MOWING_JSON_HELPER_H
#define LAWN_MOWING_JSON_HELPER_H

#include <nlohmann/json.hpp>
#include <algorithm>
#include "utils/utils.hpp"
#include "mowing/LowerBoundSolver.h"

using json = nlohmann::json;

json toJson(mowing::LowerBoundSolver::solution &solution) {

    json result;

    result["iterations"] = json::array();
    result["polygon"] = json::array();

    for (auto &sol: solution.lb_solutions) {
        json lbSolutionJson;

        lbSolutionJson["tour"] = json::array();
        lbSolutionJson["witnesses"] = json::array();
        lbSolutionJson["base_witnesses"] = json::array();
        lbSolutionJson["k_means_centroids"] = json::array();
        lbSolutionJson["tour_after_tsp"] = json::array();
        lbSolutionJson["cetsp_time"] = sol.cetsp_time;
        lbSolutionJson["tsp_time"] = sol.tsp_time;
        lbSolutionJson["optimal"] = sol.optimal;
        lbSolutionJson["lower_bound"] = sol.lower_bound;
        lbSolutionJson["strategy"] = sol.strategy;

        for (auto &p: sol.base_witnesses) {
            lbSolutionJson["base_witnesses"].emplace_back((json) {
                    {"x", CGAL::to_double(p.x())},
                    {"y", CGAL::to_double(p.y())}});
        }

        for (auto &p: sol.k_means_centroids) {
            lbSolutionJson["k_means_centroids"].emplace_back((json) {
                    {"x", CGAL::to_double(p.x())},
                    {"y", CGAL::to_double(p.y())}});
        }

        for (auto &p: sol.witnesses) {
            result["witnesses"].emplace_back((json) {
                    {"x", CGAL::to_double(p.x())},
                    {"y", CGAL::to_double(p.y())}});
        }

        for (auto &p: sol.tour) {
            lbSolutionJson["tour"].emplace_back((json) {
                    {"x", CGAL::to_double(p.x())},
                    {"y", CGAL::to_double(p.y())}});
        }

        for (auto &p: sol.tour_after_tsp) {
            lbSolutionJson["tour_after_tsp"].emplace_back((json) {
                    {"x", CGAL::to_double(p.x())},
                    {"y", CGAL::to_double(p.y())}});
        }

        result["iterations"].emplace_back(lbSolutionJson);
    }

    for (auto it = solution.polygon.vertices_begin(); it != solution.polygon.vertices_end(); it++) {
        result["polygon"].emplace_back((json) {
                {"x", CGAL::to_double(it->x())},
                {"y", CGAL::to_double(it->y())}});
    }

    result["lower_bound"] = solution.lower_bound;
    result["radius"] = solution.radius;

    return result;
}

#endif //LAWN_MOWING_JSON_HELPER_H
