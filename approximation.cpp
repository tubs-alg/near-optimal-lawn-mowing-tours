
#include <cstdlib>
#include <iostream>
#include <json/writer.h>
#include <algorithm>
#include "utils/utils.hpp"
#include "mowing/FeketeApproximation.h"

void exportJson(const std::string &out_file, mowing::FeketeApproximation::solution &solution) {
    Json::Value result;

    Json::Value polygonJson(Json::arrayValue);
    Json::Value tourJson(Json::arrayValue);
    Json::Value pointsJson(Json::arrayValue);

    for (auto it = solution.polygon.vertices_begin(); it != solution.polygon.vertices_end(); it++) {
        Json::Value point;
        point["x"] = Json::Value(CGAL::to_double(it->x()));
        point["y"] = Json::Value(CGAL::to_double(it->y()));
        polygonJson.append(point);
    }

    for (auto & it : solution.tour) {
        Json::Value point;
        point["x"] = Json::Value(CGAL::to_double(it.x()));
        point["y"] = Json::Value(CGAL::to_double(it.y()));
        tourJson.append(point);
    }

    for (auto & it : solution.points) {
        Json::Value point;
        point["x"] = Json::Value(CGAL::to_double(it.x()));
        point["y"] = Json::Value(CGAL::to_double(it.y()));
        pointsJson.append(point);
    }

    result["tour"] = tourJson;
    result["points"] = pointsJson;
    result["polygon"] = polygonJson;
    result["lower_bound"] = Json::Value(solution.lower_bound);
    result["upper_bound"] = Json::Value(solution.upper_bound);
    result["solved_optimally"] = Json::Value(solution.solved_optimally);
    result["runtime"] = Json::Value(solution.runtime);
    result["radius"] = Json::Value(solution.radius);

    std::ofstream outfile;
    outfile.open(out_file);
    outfile << result << std::endl;
    outfile.close();

    std::cout << "Export completed to " << out_file <<std::endl;
}

int main(int argc, char *argv[]) {

    if(argc != 5) {
        std::cout << "Too few or many arguments" << std::endl;
        return 0;
    }

    Polygon_2 polygon;

    std::string line;
    std::string file_name = argv[1];
    std::string out_file = argv[2];
    double time = atof(argv[3]);
    double radius = atof(argv[4]);

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
        auto solver = mowing::FeketeApproximation(polygon, radius, time);
        auto solution = solver.solve();

        std::cout << "Polygon: " << polygon << std::endl;

        exportJson(out_file, solution);

    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}