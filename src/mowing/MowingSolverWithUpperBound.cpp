#include "mowing/MowingSolverWithUpperBound.h"


namespace mowing {


    MowingSolverWithUpperBound::MowingSolverWithUpperBound(Polygon_2 &polygon, int initial_strategy,
                                                           int followup_strategy,
                                                           double radius, double time,
                                                           std::size_t max_witness_size_initial,
                                                           std::size_t max_witness_size,
                                                           std::size_t max_iterations, Kernel::FT upper_bound) :
            MowingSolver(polygon,
                         initial_strategy,
                         followup_strategy,
                         radius,
                         time,
                         max_witness_size_initial,
                         max_witness_size,
                         max_iterations), upper_bound(std::move(upper_bound)) {

    }

    MowingSolverWithUpperBound::ShortestConnectingSegmentResult
    MowingSolverWithUpperBound::getShortestConnectingSegment(PointVector &tour, Conic_Polygon_with_holes_2 &region) {
        return mowing::utils::shortest_connecting_segment(tour, region);
    }

    void MowingSolverWithUpperBound::addTourForRegion(PointVector &tour,
                                                      Conic_Polygon_with_holes_2 &region,
                                                      PointVector &all_witnesses) {

        auto shortest_connecting_result = this->getShortestConnectingSegment(tour, region);

        auto shortest_segment = shortest_connecting_result.first;
        auto edge_to_replace = shortest_connecting_result.second;

        auto start_point = std::make_shared<Point>(shortest_segment->source());

        auto subtour = this->computeTourForConic(region.outer_boundary(),
                                                 all_witnesses,
                                                 start_point);

        long start_index = 0;

        // Find index of the splitting point in the subtour (might be slightly different due to double precision)
        for (long i = 0; i < (long) subtour.size(); i++) {
            if (CGAL::has_smaller_distance_to_point(shortest_segment->source(), subtour[i], subtour[start_index])) {
                start_index = i;
            }
        }

        // Modify vector so that start vector is at front
        std::rotate(subtour.begin(), subtour.begin() + start_index, subtour.end());
        subtour[0] = *start_point;

        for (auto it = tour.begin(); it != tour.end(); it++) {

            auto next = it + 1;
            if (next == tour.end()) next = tour.begin();

            if (edge_to_replace == nullptr) {
                if (*it == shortest_segment->source()) {
                    tour.insert(it, subtour.begin(), subtour.end());
                    break;
                }
            } else if (*it == edge_to_replace->first && *next == edge_to_replace->second) {
                subtour.emplace_back(*start_point);
                tour.insert(next, subtour.begin(), subtour.end());
                break;
            }
        }

        mowing::utils::cleanup_tour(tour);
    }

    MowingSolverWithUpperBound::PointVector
    MowingSolverWithUpperBound::computeTourForConic(const ConicPolygon &P,
                                                    PointVector &all_witnesses,
                                                    std::shared_ptr<Point> &start_point) {
        auto witnesses = PointVector();

        // Add witnesses, sparsify them afterward
        this->followup_strategy->placeWitnesses(witnesses, this->max_witness_size, P);
        this->followup_sparsification->sparsify(witnesses, this->max_witness_size, P, this->straight_line_polygon);

        // Add witnesses to all_witnesses for logging
        all_witnesses.insert(all_witnesses.end(), witnesses.begin(), witnesses.end());

        // Calculate CETSP + TSP (if necessary) and return the shortest tour.
        return this->calculateCETSPWithTSPAndUpdateBounds(witnesses, start_point);
    }

    MowingSolverWithUpperBound::PointVector
    MowingSolverWithUpperBound::calculateCETSPWithTSPAndUpdateBounds(PointVector &witnesses,
                                                                     std::shared_ptr<Point> &start_point) {
        try {
            // Calculate CETSP. In case of an error we perform TSP
            auto solution = this->calculateCETSPWithTSP(witnesses, start_point);

            // Use the shorter tour.
            auto &tour =
                    solution.optimal || solution.tour_after_tsp.empty() ? solution.tour : solution.tour_after_tsp;

            // Update the lower bound to the tour length if the tour was optimal. Else update the LB according to the solvers LB
            if (solution.optimal) this->updateLowerBound(tour);
            else this->updateLowerBound(solution.lower_bound);

            return tour;
        } catch (MowingSolverCETSPException &ex) {
            // In case anything goes wrong and no tour can be computed we use TSP
            this->updateLowerBound(ex.getPartialSolution().lower_bound);
            std::cout << "Could not generate tour. Using TSP instead." << std::endl;
            auto tsp_solver = tsp::TSPSolver(witnesses, this->time);
            auto tour = tsp_solver.solve().points;
            return tour;
        }
    }

    void MowingSolverWithUpperBound::registerCallback(std::function<void(solution & )> &cb) {
        this->callback = cb;
    }

    void MowingSolverWithUpperBound::writeBoundsAndExport(solution &sol, solution_iteration &iteration) {
        iteration.lower_bound = this->getLowerBound();
        iteration.upper_bound = this->getUpperBound();

        sol.lower_bound = iteration.lower_bound;
        sol.upper_bound = iteration.upper_bound;

        if (this->callback != nullptr) this->callback(sol);
    }

    Kernel::FT MowingSolverWithUpperBound::computeGap() {
        return CGAL::abs(this->upper_bound - this->lower_bound) / this->upper_bound;
    }

    void MowingSolverWithUpperBound::updateUpperBound(PointVector &solution) {
        this->upper_bound = CGAL::min(::utils::compute_tour_length(solution), this->upper_bound);
    }

    double MowingSolverWithUpperBound::getUpperBound() {
        return CGAL::to_double(this->upper_bound);
    }
}
