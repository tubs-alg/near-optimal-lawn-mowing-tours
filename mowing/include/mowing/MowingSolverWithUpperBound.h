#ifndef MOWING_SOLVER_WITH_UB_H
#define MOWING_SOLVER_WITH_UB_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <chrono>

#include "MowingSolver.h"
#include "LowerBoundSolver.h"

namespace mowing {
    class MowingSolverWithUpperBound : public MowingSolver {
    public:
        struct solution_step {
            PointVector witness_set;
            PointVector tour;
            std::vector<Conic_Polygon_with_holes_2> uncovered_area;
            double time;
        };

        struct solution_iteration {
            std::vector<solution_step> steps;
            double lower_bound;
            double upper_bound;
            double time;
        };

        struct solution {
            std::vector<solution_iteration> iterations;

            Polygon_2 polygon;
            double radius;
            double lower_bound;
            double upper_bound;
            bool solved_optimally;
            double time;
        };

        MowingSolverWithUpperBound(Polygon_2 &polygon, int initial_strategy, int followup_strategy,
                                   double radius, double time, std::size_t max_witness_size_initial,
                                   std::size_t max_witness_size,
                                   std::size_t max_iterations, Kernel::FT upper_bound = 9999999);

        void registerCallback(std::function<void(solution &)> &cb);

        virtual solution solve() = 0;
        double getUpperBound();


    protected:
        typedef std::pair<std::shared_ptr<Segment>, std::shared_ptr<std::pair<Point, Point>>> ShortestConnectingSegmentResult;
        std::function<void(solution &)> callback;
        Kernel::FT upper_bound;

        void addTourForRegion(PointVector &tour, Conic_Polygon_with_holes_2 &region, PointVector &all_witnesses);

        virtual ShortestConnectingSegmentResult getShortestConnectingSegment(PointVector &tour, Conic_Polygon_with_holes_2 &region);

        virtual PointVector computeTourForConic(const ConicPolygon &P,
                                        PointVector &all_witnesses,
                                        std::shared_ptr<Point> &start_point);

        PointVector calculateCETSPWithTSPAndUpdateBounds(PointVector &witnesses, std::shared_ptr<Point> &start_point);

        virtual void updateUpperBound(PointVector &solution);
        Kernel::FT computeGap();
        void writeBoundsAndExport(solution &sol, solution_iteration &iteration);
    };
}

#endif
