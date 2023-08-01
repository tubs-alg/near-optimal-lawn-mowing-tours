/**
 * This file defines the python bindings.
 */
#include <iostream>
#include <string>
#include <algorithm>
#include "utils/utils.hpp"
#include "mowing/LowerBoundSolver.h"
#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/stl.h>       // automatic conversion of vectors
#include "utils/json_helper.h"
#include "pybind11_json/pybind11_json.hpp"
#include <nlohmann/json.hpp>

namespace py = pybind11;
using namespace cetsp;
using namespace cetsp::details;


nlohmann::json
mowing_lower_bound(const std::string& file_name, double radius,
                   std::size_t max_witness_size_initial, std::size_t max_witness_size,
                   std::size_t max_iterations,
                   int initial_strategy, int followup_strategy, double timelimit) {

    Polygon_2 polygon;
    std::string line;

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

    try {
        auto solver = mowing::LowerBoundSolver(polygon, initial_strategy, followup_strategy,
                                               radius, timelimit,
                                               max_witness_size_initial,
                                               max_witness_size, max_iterations);
        auto solution = solver.solve();

        std::cout << "Polygon: " << polygon << std::endl;

        return toJson(solution);

    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        json result;
        result["error"] = ex.what();
        return result;
    }
}

PYBIND11_MODULE(_mowing_bindings, m
) {
    m.def("mowing_lower_bound", &mowing_lower_bound);

    m.attr("INITIAL_STRATEGY_CH") = py::int_(mowing::definitions::INITIAL_STRATEGY_CH);
    m.attr("INITIAL_STRATEGY_VERTICES") = py::int_(mowing::definitions::INITIAL_STRATEGY_VERTICES);
    m.attr("FOLLOWUP_STRATEGY_GRID") = py::int_(mowing::definitions::FOLLOWUP_STRATEGY_GRID);
    m.attr("FOLLOWUP_STRATEGY_RANDOM") = py::int_(mowing::definitions::FOLLOWUP_STRATEGY_RANDOM);
    m.attr("FOLLOWUP_STRATEGY_SKELETON") = py::int_(mowing::definitions::FOLLOWUP_STRATEGY_SKELETON);
}
