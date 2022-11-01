#ifndef TSPN_SHORTEST_PATH_SOLVER_H
#define TSPN_SHORTEST_PATH_SOLVER_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <cstdlib>
#include <stdio.h>
#include <cfloat>
#include <math.h>

#include "util.h"
#include "close_enough_tsp/PrintFunctions.h"
#include "close_enough_tsp/Data.h"
#include "close_enough_tsp/BranchNBound.h"
#include "close_enough_tsp/SolveSocpCplex.h"
#include "close_enough_tsp/structs.h"
#include "utils/utils.hpp"

class ShortestPathSolver {
public:

    struct solution {
        double lower_bound;
        double upper_bound;
        std::vector<Point> points;
        bool optimal_solution_found;
    };

    ShortestPathSolver(Point &start_point, Point &end_point, std::vector<Point> &points, double radius, double time = 100, double ub = 999999);

    solution solve();

private:

    std::vector<Point> convertSolution(std::vector<std::vector<double>> &solution);

    std::vector<Point> points;
    std::vector<double> radii;
    std::vector<double> demands;

    double time;
    double initial_upper_bound;
};

#endif //TSPN_SHORTEST_PATH_SOLVER_H
