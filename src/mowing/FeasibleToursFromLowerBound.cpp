#include "mowing/FeasibleToursFromLowerBound.h"


namespace mowing {


    FeasibleToursFromLowerBound::FeasibleToursFromLowerBound(LBSolution &solution, double time,
                                                             std::size_t max_witness_size) :
            MowingSolverWithUpperBound(solution.polygon,
                         definitions::INITIAL_STRATEGY_VERTICES,
                         definitions::FOLLOWUP_STRATEGY_SKELETON,
                         solution.radius,
                         time,
                         max_witness_size,
                         max_witness_size,
                         solution.lb_solutions.size()), lower_bound_solution(solution) {}

    FeasibleToursFromLowerBound::solution FeasibleToursFromLowerBound::solve() {
        auto result = solution{std::vector<solution_iteration>(),
                               this->straight_line_polygon,
                               this->radius,
                               CGAL::to_double(this->lower_bound),
                               CGAL::to_double(this->upper_bound),
                               false, 0};

        bool solved;

        PointVector tour;
        auto all_witnesses = PointVector();

        auto full_start_time = Clock::now();

        for (auto &lb_solution: this->lower_bound_solution.lb_solutions) {
            try {
                tour = lb_solution.tour;
                all_witnesses = lb_solution.witnesses;

                result.iterations.push_back(solution_iteration{
                        std::vector<solution_step>(),
                        lb_solution.lower_bound,
                        999999, 0});

                auto iteration_start_time = Clock::now();

                auto &iteration = result.iterations.back();
                this->updateLowerBound(lb_solution.lower_bound);

                do {
                    auto uncovered = this->computeUncoveredRegions(tour);
                    iteration.steps.push_back(solution_step{PointVector(all_witnesses),
                                                            PointVector(tour),
                                                            uncovered, 0});

                    auto step_start_time = Clock::now();
                    solved = uncovered.empty();

                    if (solved) {
                        this->updateUpperBound(tour);
                    } else {
                        for (auto &region: uncovered) {
                            this->addTourForRegion(tour, region, all_witnesses);
                        }
                    }

                    auto step_end_time = Clock::now();
                    iteration.steps.back().time = calculateTime(step_start_time, step_end_time);

                    std::cout << "Finished step" << std::endl;
                    this->writeBoundsAndExport(result, iteration);
                } while (!solved);

                std::cout << "Found feasible solution opt between: "
                          << this->getLowerBound() << " and "
                          << this->getUpperBound() << " Gap: " <<
                          CGAL::to_double(this->computeGap()) <<
                          " solved opt " << (this->computeGap() < 1e-5)
                          << std::endl;

                this->offset_calculator->initializeUncoveredRegions();
                result.solved_optimally = this->computeGap() < 1e-5;

                auto iteration_end_time = Clock::now();
                iteration.time = calculateTime(iteration_start_time, iteration_end_time);

                this->writeBoundsAndExport(result, iteration);

            } catch (std::runtime_error &ex) {
                std::cout << "An error " << ex.what() << std::endl;
            }
        }

        result.lower_bound = this->getLowerBound();
        result.upper_bound = this->getUpperBound();

        auto full_end_time = Clock::now();
        result.time = calculateTime(full_start_time, full_end_time);

        return result;
    }
}
