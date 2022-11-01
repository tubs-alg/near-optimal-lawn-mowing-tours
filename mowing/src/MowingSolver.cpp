#include "mowing/MowingSolver.h"

namespace mowing {


    MowingSolver::MowingSolver(Polygon_2 &polygon, int initial_strategy, int followup_strategy,
                               double radius, double time, std::size_t max_witness_size_initial,
                               std::size_t max_witness_size,
                               std::size_t max_iterations) {
        this->straight_line_polygon = polygon;

        if (this->straight_line_polygon.is_clockwise_oriented()) {
            this->straight_line_polygon.reverse_orientation();
        }

        this->radius = radius;
        this->initializeOffsetCalculator();

        this->lower_bound = 0;
        switch (initial_strategy) {
            case mowing::definitions::INITIAL_STRATEGY_VERTICES:
                this->initial_strategy = std::make_unique<PolygonVerticesPlacementStrategy>(polygon, radius);
                break;
            case mowing::definitions::INITIAL_STRATEGY_CH:
                this->initial_strategy = std::make_unique<ConvexHullPlacementStrategy>(polygon, radius);
                break;
            default:
                throw std::runtime_error("The given initial_strategy " + std::to_string(initial_strategy) +
                                         " is not implemented");
        }

        switch (followup_strategy) {
            case mowing::definitions::FOLLOWUP_STRATEGY_SKELETON:
                this->followup_strategy = std::make_unique<StraightSkeletonPlacementStrategy>(polygon, radius);
                break;
            case mowing::definitions::FOLLOWUP_STRATEGY_GRID:
                this->followup_strategy = std::make_unique<GridPlacementStrategy>(polygon, radius);
                break;
            case mowing::definitions::FOLLOWUP_STRATEGY_RANDOM:
                this->followup_strategy = std::make_unique<RandomPlacementStrategy>(polygon, radius);
                break;
            default:
                throw std::runtime_error("The given strategy " + std::to_string(followup_strategy) +
                                         " is not implemented");
        }

        this->initial_sparsification = std::make_unique<DispersionSparsification>();
        this->followup_sparsification = std::make_unique<KMeansSparsification>();

        this->initial_strategy_code = initial_strategy;
        this->followup_strategy_code = followup_strategy;

        this->max_witness_size = max_witness_size;
        this->max_witness_size_initial = max_witness_size_initial;
        this->max_iterations = max_iterations;

        this->time = time;
    }

    /**
     * Initializes the offset calculator. Note that the offset calculator must be reset for consecutive queries.
     * It preserves the uncovered regions otherwise.
     */
    void MowingSolver::initializeOffsetCalculator() {
        this->offset_calculator = std::make_unique<mowing::ExactOffsetCalculator>(this->straight_line_polygon,
                                                                                  this->radius + 1e-3, false);
    }

    /**
     * Computes the uncovered regions from the exact offset calculator
     * @param tour The tour that should be mowed before extraction.
     * @return The remaining regions after removing the Minkwoski Sum of the tour and the cutter.
     */
    MowingSolver::ConicPolygonVector
    MowingSolver::computeUncoveredRegions(PointVector &tour) {
        this->offset_calculator->computeUncoveredRegions(tour);

        auto result = ExactOffsetCalculator::ConicPolygonVector();
        this->offset_calculator->polygon_set.polygons_with_holes(std::back_inserter(result));
        return result;
    }

    /**
     * To allow a constant amount of witnesses to be placed it might be necessary to distribute the witnesses
     * across all uncovered regions. This method distributes depending on the region sizes.
     * @param polygons The uncovered regions.
     * @param n The total amount of witnesses.
     * @return A map of the form <index, count> that assigns a count to a region with SUM(c for all _, c in map)==n
     */
    std::map<std::size_t, std::size_t>
    MowingSolver::calculateWitnessCountsForRegions(ConicPolygonVector &polygons, std::size_t n) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<double> probabilities;
        std::map<std::size_t, std::size_t> counts;

        int idx = 0;
        for (auto &region: polygons) {
            counts[idx++] = 0;
            Polygon_2 poly = mowing::utils::approximate_polygon(region.outer_boundary());

            if (poly.is_simple()) {
                probabilities.emplace_back(CGAL::to_double(CGAL::abs(poly.area())));
            } else if (!probabilities.empty()) {
                probabilities.emplace_back(
                        std::accumulate(probabilities.begin(), probabilities.end(), 0.0) /
                        (double) probabilities.size());
            } else {
                probabilities.emplace_back(1);
            }
        }
        std::discrete_distribution<> d(probabilities.begin(), probabilities.end());
        for (std::size_t i = 0; i < n; ++i) {
            ++counts[d(gen)];
        }

        return counts;
    }

    /**
     * Allows for simpler access to the calculateCETSPWithTSP method. Necessary because of the shared pointer l-value.
     * @param witnesses The witness set.
     * @return A CETSP solution.
     */
    MowingSolver::cetsp_extended_solution MowingSolver::calculateCETSPWithTSP(PointVector &witnesses) {
        auto empty_start = std::shared_ptr<Point>();
        return this->calculateCETSPWithTSP(witnesses, empty_start);
    }

    /**
     * Solves CETSP with a given set of witnesses within the time that was set for the solver object.
     * In case CETSP is not optimal we create a TSP instance from the partial CETSP solution by stabbing the partial
     * tour once in every witness. An optimal TSP tour on this point set is also feasible for CETSP and should be
     * shorter for most instances. If the tour has to start in a single point, i.e. a point p with radius r_p=0
     * a start point can be set.
     * @param witnesses The witnesses
     * @param start_point The start point of the tour. Can be empty.
     * @return A CETSP solution
     */
    MowingSolver::cetsp_extended_solution
    MowingSolver::calculateCETSPWithTSP(PointVector &witnesses, [[maybe_unused]] std::shared_ptr<Point> &start_point) {

        // Solve CETSP and measure the time
        auto startTimeSolver = Clock::now();
        // If the start point is given we pass it as an initial point. Else use the default solver without a start.
        auto solver = start_point ? CETSPSolver(witnesses, *start_point, this->radius, this->time) :
                      CETSPSolver(witnesses, this->radius, this->time);
        auto solution = solver.solve();
        auto endTimeSolver = Clock::now();

        auto &tour = solution.points;

        auto extended_solution = cetsp_extended_solution{
                calculateTime(startTimeSolver, endTimeSolver),
                0,
                PointVector(witnesses),
                PointVector(tour),
                PointVector(),
                solution.optimal_solution_found,
                solution.lower_bound};

        if (tour.empty()) {
            throw MowingSolverCETSPException("CETSP yielded an empty tour.", extended_solution);
        }

        // We will now clean up the tour, meaning we will remove consecutive duplicate points
        // If all points are equal the clean up will lead to a single point tour.
        mowing::utils::cleanup_tour(tour);

        // We calculate the tour length with an exact method instead of using the provided lower bound for better precision
        // This is only possible if the given tour is optimal
        auto length = solution.optimal_solution_found ? ::utils::compute_tour_length(tour) : solution.lower_bound;
        this->updateLowerBound(length);
        extended_solution.lower_bound = CGAL::to_double(length);

        // In case the solution was not optimal we improve the calculated solution by stabbing every witness
        // and solving tsp on the resulting set of points.
        if (!solution.optimal_solution_found && tour.size() > 2) {
            auto startTimeTSPSolver = Clock::now();
            auto tsp_solver = tsp::TSPSolver(witnesses, tour, this->time);
            auto new_tour = tsp_solver.solve().points;
            auto endTimeTSPSolver = Clock::now();

            std::cout << "Improved cleanup tour. Old tour length " << ::utils::compute_tour_length(tour) << " vs. new "
                      << ::utils::compute_tour_length(new_tour) << std::endl;

            if (::utils::compute_tour_length(tour) > ::utils::compute_tour_length(new_tour)) {
                extended_solution.tsp_time = calculateTime(startTimeTSPSolver, endTimeTSPSolver);
                extended_solution.tour_after_tsp = new_tour;
            }
        }

        return extended_solution;
    }

    /**
     * Calculates the total runtime in milliseconds given a start and end point.
     * @param start The start time point.
     * @param end The end time point.
     * @return The milliseconds between both time points.
     */
    double MowingSolver::calculateTime(::utils::lvalue_or_rvalue<Clock::time_point> start,
                                       ::utils::lvalue_or_rvalue<Clock::time_point> end) {
        using std::chrono::milliseconds;
        return ((chrono::duration<double, std::milli>) (*end - *start)).count();
    }

    void MowingSolver::updateLowerBound(const Kernel::FT &lb) {
        this->lower_bound = CGAL::max(lb, this->lower_bound);
    }

    void MowingSolver::updateLowerBound(double lb) {
        this->updateLowerBound((Kernel::FT) lb);
    }

    void MowingSolver::updateLowerBound(PointVector &solution) {
        this->updateLowerBound(::utils::compute_tour_length(solution));
    }

    double MowingSolver::getLowerBound() {
        return CGAL::to_double(this->lower_bound);
    }
}

