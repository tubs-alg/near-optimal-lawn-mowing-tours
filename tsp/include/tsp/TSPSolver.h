#ifndef TSP_SOLVER_H
#define TSP_SOLVER_H

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>

#include "algutil/upper_triagular_indexing.hpp"
#include "gurobi_c++.h"
#include "utils/utils.hpp"
#include "mowing/utils/mowing.h"
#include "Christofides.h"


namespace mowing {

    namespace tsp {

        void findsubtour(int n, double **sol, int *tourlenP, int *tour);

        class SubtourElimination : public GRBCallback {
        public:
            GRBVar **vars;
            std::size_t n;

            SubtourElimination(GRBVar **vars, std::size_t n);

        protected:
            void callback();
        };

        class TSPSolver {

        public:

            struct solution {
                std::vector<Point> points;
                std::vector<std::size_t> indices;
                double lower_bound;
                double upper_bound;
                double time;
                double time_limit;
                bool solved_optimally;
            };

            TSPSolver(const std::vector<Point> &witnesses,
                      const std::vector<Point>& tour_with_intersections, double time);

            TSPSolver(std::vector<Point> &points, double time);


            solution solve();
        private:
            std::size_t n;

            double time;

            GRBEnv *env;
            GRBModel model;
            GRBVar **vars;

            std::vector<Point> points;

            void initialize_ip();
        };
    }

}

#endif //CETSP_SOLVER_H
