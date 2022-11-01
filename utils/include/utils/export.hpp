#ifndef LAWN_MOWING_EXPORT_H
#define LAWN_MOWING_EXPORT_H
#include <stdlib.h>
#include <iostream>
#include <string>
#include <json/writer.h>
#include <json/json.h>
#include <algorithm>
#include "utils/utils.hpp"
#include "mowing/FeasibleToursFromLowerBound.h"
#include "mowing/LowerBoundSolver.h"

namespace utils {
    void exportJson(std::string out_file, mowing::FeasibleToursFromLowerBound::solution &solution) {
        Json::Value result;

        Json::Value iterations(Json::arrayValue);
        Json::Value polygonJson(Json::arrayValue);


        for (auto &iter: solution.iterations) {
            Json::Value iteration;
            Json::Value iterationSteps(Json::arrayValue);

            for (auto &step: iter.steps) {
                Json::Value iterationStep;
                Json::Value witnessesJson(Json::arrayValue);
                Json::Value tourJson(Json::arrayValue);
                Json::Value uncoveredPolygons(Json::arrayValue);

                for (auto &p: step.witness_set) {
                    Json::Value point;
                    point["x"] = Json::Value(CGAL::to_double(p.x()));
                    point["y"] = Json::Value(CGAL::to_double(p.y()));
                    witnessesJson.append(point);
                }

                for (auto &p: step.tour) {
                    Json::Value point;
                    point["x"] = Json::Value(CGAL::to_double(p.x()));
                    point["y"] = Json::Value(CGAL::to_double(p.y()));
                    tourJson.append(point);
                }

                for (auto &pol: step.uncovered_area) {
                    Json::Value poly;

                    auto outer_boundary = pol.outer_boundary();
                    if (outer_boundary.orientation() != CGAL::COUNTERCLOCKWISE) outer_boundary.reverse_orientation();

                    for (auto it = outer_boundary.curves_begin(); it != outer_boundary.curves_end(); it++) {

                        auto points = std::vector<std::pair<double, double>>();

                        it->polyline_approximation(10, std::back_inserter(points));

                        if (!it->is_directed_right()) {
                            std::reverse(points.begin(), points.end());
                        }

                        points.pop_back();
                        for (auto &p: points) {
                            Json::Value point;
                            point["x"] = Json::Value(p.first);
                            point["y"] = Json::Value(p.second);

                            poly.append(point);
                        }
                    }

                    uncoveredPolygons.append(poly);
                }

                iterationStep["tour"] = tourJson;
                iterationStep["witnesses"] = witnessesJson;
                iterationStep["uncoveredPolygons"] = uncoveredPolygons;
                iterationStep["time"] = Json::Value(step.time);

                iterationSteps.append(iterationStep);
            }

            iteration["steps"] = iterationSteps;
            iteration["lower_bound"] = Json::Value(iter.lower_bound);
            iteration["upper_bound"] = Json::Value(iter.upper_bound);
            iteration["time"] = Json::Value(iter.time);

            iterations.append(iteration);
        }

        for (auto it = solution.polygon.vertices_begin(); it != solution.polygon.vertices_end(); it++) {
            Json::Value point;
            point["x"] = Json::Value(CGAL::to_double(it->x()));
            point["y"] = Json::Value(CGAL::to_double(it->y()));
            polygonJson.append(point);
        }


        result["polygon"] = polygonJson;
        result["iterations"] = iterations;
        result["lower_bound"] = Json::Value(solution.lower_bound);
        result["upper_bound"] = Json::Value(solution.upper_bound);
        result["radius"] = Json::Value(solution.radius);
        result["time"] = Json::Value(solution.time);

        std::ofstream outfile;
        outfile.open(out_file);
        outfile << result << std::endl;
        outfile.close();

        std::cout << "Export completed to " << out_file <<std::endl;
    }
}

#endif //LAWN_MOWING_EXPORT_H
