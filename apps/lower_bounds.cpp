#include <cstdlib>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "utils/utils.hpp"
#include "mowing/LowerBoundSolver.h"
#include "utils/json_helper.h"

void exportJson(const std::string &out_file, mowing::LowerBoundSolver::solution &solution) {
    auto result = toJson(solution);

    std::ofstream outfile;
    outfile.open(out_file);
    outfile << result << std::endl;
    outfile.close();

    std::cout << "Export completed to " << out_file << std::endl;
}

int main(int argc, char *argv[]) {

    if (argc != 10) {
        std::cout << "Too few or many arguments" << std::endl;
        return 0;
    }


    Polygon_2 polygon;

    std::string line;
    std::string file_name = argv[1];
    std::string out_file = argv[2];
    auto initial_strategy = (int) std::stoul(argv[3]);
    auto followup_strategy = (int) std::stoul(argv[4]);
    double time = atof(argv[5]);
    double radius = atof(argv[6]);
    auto max_witness_size_initial = (std::size_t) std::stoul(argv[7]);
    auto max_witness_size = (std::size_t) std::stoul(argv[8]);
    auto max_iterations = (std::size_t) std::stoul(argv[9]);

    std::ifstream input_file(file_name);
    if (!input_file.is_open()) {
        std::cout << "Could not open file " << file_name << std::endl;
        return 0;
    }
    std::getline(input_file, line);
    input_file.close();

    std::stringstream ss;
    ss << line;
    ss >> polygon;
    std::cout << "Input polygon " << file_name << " has " << polygon.size() << " inside_points" << std::endl;
    std::cout << "Input polygon is simple " << polygon.is_simple() << " and has area " << polygon.area() << std::endl;

    auto solver = mowing::LowerBoundSolver(polygon, initial_strategy, followup_strategy,
                                           radius, time,
                                           max_witness_size_initial,
                                           max_witness_size, max_iterations);
    auto solution = solver.solve();

    std::cout << "Polygon: " << polygon << std::endl;

    exportJson(out_file, solution);

}