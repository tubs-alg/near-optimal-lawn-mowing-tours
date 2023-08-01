#ifndef MOWING_SOLVER_H
#define MOWING_SOLVER_H

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
#include "cetsp/solver.h"
#include "ExactOffsetCalculator.h"
#include "mowing/utils/mowing.h"
#include "tsp/TSPSolver.h"
#include "mowing/utils/cetsp_tours.h"
#include "mowing/utils/definitions.h"
#include "mowing/utils/conversion.h"
#include "mowing/witnesses/placement/WitnessPlacementStrategy.h"
#include "mowing/witnesses/placement/initial/ConvexHullPlacementStrategy.h"
#include "mowing/witnesses/placement/initial/PolygonVerticesPlacementStrategy.h"
#include "mowing/witnesses/placement/followup/RandomPlacementStrategy.h"
#include "mowing/witnesses/placement/followup/GridPlacementStrategy.h"
#include "mowing/witnesses/placement/followup/StraightSkeletonPlacementStrategy.h"
#include "mowing/witnesses/sparsification/WitnessSparsification.h"
#include "mowing/witnesses/sparsification/DispersionSparsification.h"
#include "mowing/witnesses/sparsification/KMeansSparsification.h"

namespace mowing {
    class MowingSolver {
    public:
        typedef mowing::definitions::PointVector PointVector;
        typedef mowing::definitions::ConicPolygonVector ConicPolygonVector;
        typedef mowing::definitions::Conic_Polygon_with_holes_2 Conic_Polygon_with_holes_2;
        typedef mowing::definitions::ConicPolygon ConicPolygon;
        typedef mowing::definitions::InputPolygon InputPolygon;
        typedef mowing::definitions::Clock Clock;


        struct cetsp_extended_solution {
            double cetsp_time{};
            double tsp_time{};
            PointVector witnesses;
            PointVector tour;
            PointVector tour_after_tsp;
            bool optimal{};
            double lower_bound{};
        };

        class MowingSolverCETSPException : public std::exception {
        private:
            std::string message_;
            cetsp_extended_solution partial_solution_;
        public:
            explicit MowingSolverCETSPException(std::string message, cetsp_extended_solution &partial_solution) :
                    message_(std::move(message)), partial_solution_(partial_solution) {}

            MowingSolverCETSPException(const MowingSolverCETSPException &other) noexcept {
                this->message_ = other.message_;
                this->partial_solution_ = other.partial_solution_;
            }

            [[nodiscard]] const char *what() const noexcept override {
                return message_.c_str();
            }

            cetsp_extended_solution getPartialSolution() { return partial_solution_; }
        };

        MowingSolver(Polygon_2 &polygon, int initial_strategy, int followup_strategy,
                     double radius, double time, std::size_t max_witness_size_initial,
                     std::size_t max_witness_size,
                     std::size_t max_iterations);

        double getLowerBound();

    protected:
        Polygon_2 straight_line_polygon;

        std::unique_ptr<mowing::ExactOffsetCalculator> offset_calculator;

        double radius;
        std::unique_ptr<WitnessPlacementStrategy> initial_strategy;
        std::unique_ptr<WitnessPlacementStrategy> followup_strategy;

        std::unique_ptr<WitnessSparsification> initial_sparsification;
        std::unique_ptr<WitnessSparsification> followup_sparsification;

        int initial_strategy_code;
        int followup_strategy_code;

        Kernel::FT lower_bound;

        double time;

        std::size_t max_witness_size;
        std::size_t max_witness_size_initial;
        std::size_t max_iterations;

        void initializeOffsetCalculator();

        virtual ConicPolygonVector computeUncoveredRegions(PointVector &tour);

        static std::map<std::size_t, std::size_t>
        calculateWitnessCountsForRegions(ConicPolygonVector &polygons, std::size_t n);

        cetsp_extended_solution
        calculateCETSPWithTSP(PointVector &witnesses, std::shared_ptr<Point> &start_point);

        cetsp_extended_solution
        calculateCETSPWithTSP(PointVector &witnesses);

        static double calculateTime(::utils::lvalue_or_rvalue<Clock::time_point> start,
                                    ::utils::lvalue_or_rvalue<Clock::time_point> end);

        void updateLowerBound(double lb);
        void updateLowerBound(PointVector &solution);

        virtual void updateLowerBound(const Kernel::FT &lb);
    };
}

#endif
