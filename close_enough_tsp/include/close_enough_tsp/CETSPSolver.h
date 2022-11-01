#ifndef TSPN_CETSPSOLVER_H
#define TSPN_CETSPSOLVER_H

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

class CETSPSolver {
public:

    struct solution {
        double lower_bound;
        double upper_bound;
        std::vector<Point> points;
        bool optimal_solution_found;
    };

    CETSPSolver(std::vector<Point> &points, double radius, double time = 100, double ub = 999999);
    CETSPSolver(std::vector<Point> &points, Point &start_point, double radius, double time = 100, double ub = 999999);

    solution solve();

private:

    std::vector<Point> convertSolution(std::vector<std::vector<double>> &solution);

    std::vector<Point> points;
    std::vector<double> radii;
    std::vector<double> demands;

    double time;
    double initial_upper_bound;
};

#endif //TSPN_CETSPSOLVER_H
