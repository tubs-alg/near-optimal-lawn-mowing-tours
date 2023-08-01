#ifndef FEASIBLE_TOURS_FROM_LB_H
#define FEASIBLE_TOURS_FROM_LB_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <chrono>

#include "MowingSolverWithUpperBound.h"
#include "LowerBoundSolver.h"

namespace mowing {
    class FeasibleToursFromLowerBound : public MowingSolverWithUpperBound {
    public:
        typedef LowerBoundSolver::solution LBSolution;

        FeasibleToursFromLowerBound(LBSolution &solution, double time, std::size_t max_witness_size);

        solution solve() override;
    protected:
        LBSolution lower_bound_solution;
    };
}

#endif
