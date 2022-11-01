
#include <stdlib.h>
#include <iostream>
#include <string>
#include <json/writer.h>
#include <algorithm>
#include "utils/utils.hpp"
#include "mowing/LowerBoundSolver.h"

void exportJson(const std::string &out_file, mowing::LowerBoundSolver::solution &solution) {
    Json::Value result;

    Json::Value polygonJson(Json::arrayValue);
    Json::Value lbSolutionsJson(Json::arrayValue);

    for(auto &sol: solution.lb_solutions) {
        Json::Value lbSolutionJson;

        Json::Value witnessesJson(Json::arrayValue);
        Json::Value baseWitnessesJson(Json::arrayValue);
        Json::Value centroidsJson(Json::arrayValue);
        Json::Value tourJson(Json::arrayValue);
        Json::Value tourAfterTSPJson(Json::arrayValue);

        for (auto &p: sol.base_witnesses) {
            Json::Value point;
            point["x"] = Json::Value(CGAL::to_double(p.x()));
            point["y"] = Json::Value(CGAL::to_double(p.y()));
            baseWitnessesJson.append(point);
        }

        for (auto &p: sol.k_means_centroids) {
            Json::Value point;
            point["x"] = Json::Value(CGAL::to_double(p.x()));
            point["y"] = Json::Value(CGAL::to_double(p.y()));
            centroidsJson.append(point);
        }

        for (auto &p: sol.witnesses) {
            Json::Value point;
            point["x"] = Json::Value(CGAL::to_double(p.x()));
            point["y"] = Json::Value(CGAL::to_double(p.y()));
            witnessesJson.append(point);
        }

        for (auto &p: sol.tour) {
            Json::Value point;
            point["x"] = Json::Value(CGAL::to_double(p.x()));
            point["y"] = Json::Value(CGAL::to_double(p.y()));
            tourJson.append(point);
        }

        for (auto &p: sol.tour_after_tsp) {
            Json::Value point;
            point["x"] = Json::Value(CGAL::to_double(p.x()));
            point["y"] = Json::Value(CGAL::to_double(p.y()));
            tourAfterTSPJson.append(point);
        }

        lbSolutionJson["tour"] = tourJson;
        lbSolutionJson["cetsp_time"] = Json::Value(sol.cetsp_time);
        lbSolutionJson["tsp_time"] = Json::Value(sol.tsp_time);
        lbSolutionJson["tour_after_tsp"] = tourAfterTSPJson;
        lbSolutionJson["optimal"] = Json::Value(sol.optimal);
        lbSolutionJson["lower_bound"] = Json::Value(sol.lower_bound);
        lbSolutionJson["witnesses"] = witnessesJson;
        lbSolutionJson["base_witnesses"] = baseWitnessesJson;
        lbSolutionJson["k_means_centroids"] = centroidsJson;
        lbSolutionJson["strategy"] = Json::Value(sol.strategy);

        lbSolutionsJson.append(lbSolutionJson);
    }

    for (auto it = solution.polygon.vertices_begin(); it != solution.polygon.vertices_end(); it++) {
        Json::Value point;
        point["x"] = Json::Value(CGAL::to_double(it->x()));
        point["y"] = Json::Value(CGAL::to_double(it->y()));
        polygonJson.append(point);
    }

    result["iterations"] = lbSolutionsJson;
    result["polygon"] = polygonJson;
    result["lower_bound"] = Json::Value(solution.lower_bound);
    result["radius"] = Json::Value(solution.radius);

    std::ofstream outfile;
    outfile.open(out_file);
    outfile << result << std::endl;
    outfile.close();

    std::cout << "Export completed to " << out_file <<std::endl;
}

int main(int argc, char *argv[]) {

    if(argc != 10) {
        std::cout << "Too few or many arguments" << std::endl;
        return 0;
    }


    Polygon_2 polygon;

    std::string line;
    std::string file_name = argv[1];
    std::string out_file = argv[2];
    auto initial_strategy = (int) stoul(argv[3]);
    auto followup_strategy = (int) stoul(argv[4]);
    double time = atof(argv[5]);
    double radius = atof(argv[6]);
    auto max_witness_size_initial = (std::size_t) stoul(argv[7]);
    auto max_witness_size = (std::size_t) stoul(argv[8]);
    auto max_iterations = (std::size_t) stoul(argv[9]);

    std::ifstream input_file (file_name);
    if (!input_file.is_open()) {
        std::cout << "Could not open file " << file_name << std::endl;
        return 0;
    }
    std::getline (input_file,line);
    input_file.close();

    std::stringstream ss;
    ss << line;
    ss >> polygon;
    std::cout << "Input polygon " << file_name << " has " << polygon.size() << " inside_points" << std::endl;
    std::cout << "Input polygon is simple " << polygon.is_simple() << " and has area " << polygon.area() << std::endl;

    try {
        auto solver = mowing::LowerBoundSolver(polygon, initial_strategy, followup_strategy,
                                               radius, time,
                                               max_witness_size_initial,
                                               max_witness_size, max_iterations);
        auto solution = solver.solve();

        std::cout << "Polygon: " << polygon << std::endl;

        exportJson(out_file, solution);

    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}