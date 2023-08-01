#include "mowing/LowerBoundSolver.h"

namespace mowing {


    LowerBoundSolver::LowerBoundSolver(Polygon_2 &polygon, int initial_strategy, int followup_strategy,
                                       double radius, double time, std::size_t max_witness_size_initial,
                                       std::size_t max_witness_size,
                                       std::size_t max_iterations) :
                                       MowingSolver(polygon, initial_strategy, followup_strategy, radius,
                                                    time, max_witness_size_initial, max_witness_size, max_iterations){}

    LowerBoundSolver::solution LowerBoundSolver::solve() {
        auto result = solution{this->straight_line_polygon,
                               this->radius,
                               CGAL::to_double(this->lower_bound),
                               std::vector<lower_bound_solution>(), 0};

        auto full_start_time = Clock::now();

        try {
            auto all_witnesses = PointVector(); // Holds the witnesses for all iterations

            for (std::size_t i = 0; i < this->max_iterations; i++) {
                auto centroids = PointVector();
                auto iteration_witnesses = PointVector();

                int strategy = i == 0 ? this->initial_strategy_code : this->followup_strategy_code;

                if (i == 0) {
                    this->initial_strategy->placeWitnesses(all_witnesses, std::nullopt, std::nullopt);
                    iteration_witnesses.insert(iteration_witnesses.end(), all_witnesses.begin(), all_witnesses.end());
                    this->initial_sparsification->sparsify(all_witnesses, this->max_witness_size_initial);
                } else {
                    auto tour = result.lb_solutions.back().tour_after_tsp.empty() ? result.lb_solutions.back().tour :
                                result.lb_solutions.back().tour_after_tsp;

                    auto uncovered = this->computeUncoveredRegions(tour);
                    auto witness_counts_for_region = calculateWitnessCountsForRegions(uncovered, this->max_witness_size);

                    for (std::size_t j = 0; j < uncovered.size(); j++) {
                        auto &region = uncovered[j];
                        std::size_t witness_count = witness_counts_for_region[j];
                        auto boundary = region.outer_boundary();
                        auto witnesses = PointVector();
                        this->followup_strategy->placeWitnesses(witnesses, witness_count, boundary);

                        // Add witnesses to iteration
                        iteration_witnesses.insert(iteration_witnesses.end(), witnesses.begin(), witnesses.end());

                        // Sparsification, save centroids if needed
                        auto current_centroids = this->followup_sparsification->sparsify(witnesses, witness_count, boundary);
                        if (current_centroids) {
                            centroids.insert(centroids.end(), current_centroids->begin(), current_centroids->end());
                        }

                        // Add witnesses to all witnesses.
                        all_witnesses.insert(all_witnesses.end(), witnesses.begin(), witnesses.end());
                    }

                    std::cout << all_witnesses.size() << " witness size" << std::endl;
                }

                result.lb_solutions.emplace_back(lower_bound_solution{
                        0,
                        0,
                        strategy,
                        PointVector(all_witnesses),
                        PointVector(centroids),
                        PointVector(iteration_witnesses),
                        PointVector(),
                        PointVector(), false, 0});
                this->getOptimalCETSPTour(result.lb_solutions.back());
                result.lower_bound = this->getLowerBound();
                this->offset_calculator->initializeUncoveredRegions();
            }
        } catch (std::runtime_error &ex) {
            result.lower_bound = this->getLowerBound();
            std::cout << "An error " << ex.what() << std::endl;
        } catch (GRBException &ex) {
            result.lower_bound = this->getLowerBound();
            std::cout << "A gurobi error occured " << ex.getErrorCode() << ": " << ex.getMessage() << std::endl;
        } catch (MowingSolver::MowingSolverCETSPException &ex) {
            this->updateLowerBound(ex.getPartialSolution().lower_bound);
            std::cout << "Ending computation early" << std::endl;
        }

        auto full_end_time = Clock::now();
        result.time += calculateTime(full_start_time, full_end_time);

        return result;
    }

    void LowerBoundSolver::getOptimalCETSPTour(lower_bound_solution &lb_solution) {

        try{
            auto solution = this->calculateCETSPWithTSP(lb_solution.witnesses);

            lb_solution.tour = solution.tour;
            lb_solution.tour_after_tsp = solution.tour_after_tsp;

            lb_solution.witnesses = solution.witnesses;

            lb_solution.tsp_time = solution.tsp_time;
            lb_solution.cetsp_time = solution.cetsp_time;
            lb_solution.lower_bound = solution.lower_bound;
            lb_solution.optimal = solution.optimal;
        } catch (MowingSolver::MowingSolverCETSPException &ex) {
            this->updateLowerBound(ex.getPartialSolution().lower_bound);
            throw ex;
        }
    }
}

