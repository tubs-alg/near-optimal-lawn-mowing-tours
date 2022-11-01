#ifndef LAWN_MOWING_IMPORT_HPP
#define LAWN_MOWING_IMPORT_HPP

#include "mowing/LowerBoundSolver.h"
#include <json/writer.h>
#include <json/json.h>
#include <iostream>
#include <string>
#include <algorithm>


namespace utils {
    mowing::LowerBoundSolver::solution importJsonLB(std::string &file) {
        Json::Value sol;
        std::ifstream sol_file(file);
        sol_file >> sol;

        auto arrayToPointVector = [](Json::Value &array) {
            std::vector<Point> points;
            for(auto &p: array) {
                points.emplace_back(p["x"].asDouble(), p["y"].asDouble());
            }
            return points;
        };

        auto polygonVector = arrayToPointVector(sol["polygon"]);

        mowing::LowerBoundSolver::solution solution{
                Polygon_2(polygonVector.begin(), polygonVector.end()),
                sol["radius"].asDouble(),
                sol["lower_bound"].asDouble(),
                std::vector<mowing::LowerBoundSolver::lower_bound_solution>()
        };

        for(auto &iteration: sol["iterations"]) {
            solution.lb_solutions.emplace_back(mowing::LowerBoundSolver::lower_bound_solution{
                    iteration["cetsp_time"].asDouble(),
                    iteration["tsp_time"].asDouble(),
                    iteration["strategy"].asInt(),
                    arrayToPointVector(iteration["witnesses"]),
                    arrayToPointVector(iteration["k_means_centroids"]),
                    arrayToPointVector(iteration["base_witnesses"]),
                    arrayToPointVector(iteration["tour"]),
                    arrayToPointVector(iteration["tour_after_tsp"]),
                    iteration["optimal"].asBool(),
                    iteration["lower_bound"].asDouble()
            });
        }

        return solution;
    }


    mowing::FeasibleToursFromLowerBound::solution importSolutionJson(std::string &file) {
        using namespace mowing;

        Json::Value sol;
        std::ifstream sol_file(file);
        sol_file >> sol;

        auto arrayToPointVector = [](Json::Value &array) {
            std::vector<Point> points;
            for (auto &p: array) {
                points.emplace_back(p["x"].asDouble(), p["y"].asDouble());
            }
            return points;
        };

        auto convertPolygonWithHoles = [&arrayToPointVector](Json::Value &array) {
            typedef ExactOffsetCalculator::Traits::Curve_2 Curve_2;
            typedef ExactOffsetCalculator::Traits::Rat_segment_2 Segment_2;
            typedef ExactOffsetCalculator::Traits::Point_2 Point_2;
            typedef ExactOffsetCalculator::Gps_traits::Point_2 Conic_Point;

            auto pVector = arrayToPointVector(array);
            auto polygon = Polygon_2(pVector.begin(), pVector.end());

            ExactOffsetCalculator::Polygon_2 converted_polygon;

            for (auto it = polygon.edges_begin(); it != polygon.edges_end(); it++) {
                auto source = it->source();
                auto target = it->target();
                auto convertedSource = ExactOffsetCalculator::Point(CGAL::to_double(source.x()), CGAL::to_double(source.y()));;
                auto convertedTarget = ExactOffsetCalculator::Point(CGAL::to_double(target.x()), CGAL::to_double(target.y()));;

                if(convertedSource == convertedTarget) continue;

                auto circularArc = Curve_2(Segment_2(convertedSource, convertedTarget));

                auto xMonotoneConstructor = ExactOffsetCalculator::Traits::Make_x_monotone_2();
                auto xMonotoneCurves = std::vector<boost::variant<Conic_Point, X_monotone_curve_2>>();

                xMonotoneConstructor(circularArc, std::back_inserter(xMonotoneCurves));
                for (auto &val: xMonotoneCurves) {
                    if (val.which() == 1) {
                        converted_polygon.push_back(boost::get<X_monotone_curve_2>(val));
                    }
                }
            }

            return converted_polygon;
        };

        auto polygonVector = arrayToPointVector(sol["polygon"]);

        FeasibleToursFromLowerBound::solution solution{
                std::vector<FeasibleToursFromLowerBound::solution_iteration>(),
                Polygon_2(polygonVector.begin(), polygonVector.end()),
                sol["radius"].asDouble(),
                sol["lower_bound"].asDouble(),
                sol["upper_bound"].asDouble(),
                sol["solved_optimally"].asBool(),
                sol["time"].asDouble(),
        };

        for (auto &iteration: sol["iterations"]) {
            /*
             * std::vector<solution_step> steps;
                double lower_bound;
                double upper_bound;
                double time;

                TSPNMowingSolver::PointVector witness_set;
                TSPNMowingSolver::PointVector tour;
                std::vector<Conic_Polygon_with_holes_2> uncovered_area;
                double time;
             * */
            solution.iterations.emplace_back(FeasibleToursFromLowerBound::solution_iteration{
                    std::vector<FeasibleToursFromLowerBound::solution_step>(),
                    iteration["lower_bound"].asDouble(),
                    iteration["upper_bound"].asDouble(),
                    iteration["time"].asDouble()
            });

            for (auto &step: iteration["steps"]) {
                solution.iterations.back().steps.emplace_back(FeasibleToursFromLowerBound::solution_step{
                        arrayToPointVector(step["witnesses"]),
                        arrayToPointVector(step["tour"]),
                        std::vector<FeasibleToursFromLowerBound::Conic_Polygon_with_holes_2>(),
                        step["time"].asDouble()
                });

                auto offsetCalculator = ExactOffsetCalculator(solution.polygon, solution.radius);


                for(auto reg: step["uncoveredPolygons"]) {
                    solution.iterations.back().steps.back().uncovered_area.emplace_back(convertPolygonWithHoles(reg));
                }
            }
        }

        return solution;
    }

}

#endif //LAWN_MOWING_IMPORT_HPP
