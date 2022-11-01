#ifndef MOWING_LOWER_BOUNDS_SOLVER_H
#define MOWING_LOWER_BOUNDS_SOLVER_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <utility>
#include <chrono>

#include "utils/utils.hpp"
#include "utils/tours.hpp"
#include "utils/polygons.hpp"
#include "utils/cgal.h"
#include "close_enough_tsp/CETSPSolver.h"
#include "mowing/utils/mowing.h"
#include "MowingSolver.h"
#include "mowing/utils/cetsp_tours.h"

namespace mowing {
    class LowerBoundSolver : public MowingSolver {
    public:
        struct lower_bound_solution {
            double cetsp_time;
            double tsp_time;
            int strategy;
            PointVector witnesses;
            PointVector k_means_centroids;
            PointVector base_witnesses;
            LowerBoundSolver::PointVector tour;
            LowerBoundSolver::PointVector tour_after_tsp;
            bool optimal;
            double lower_bound;
        };

        struct solution {
            Polygon_2 polygon;
            double radius;
            double lower_bound;
            std::vector<lower_bound_solution> lb_solutions;
            double time;
        };

        LowerBoundSolver(Polygon_2 &polygon,
                         int initial_strategy, int followup_strategy,
                         double radius, double time,
                         std::size_t max_witness_size_initial,
                         std::size_t max_witness_size,
                         std::size_t max_iterations);

        solution solve();

    protected:

        void getOptimalCETSPTour(lower_bound_solution &lb_solution);
    };
}

#endif
