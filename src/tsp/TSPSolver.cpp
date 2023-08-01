#include "tsp/TSPSolver.h"

namespace tsp {
    SubtourElimination::SubtourElimination(GRBVar **vars, std::size_t n) {
        this->vars = vars;
        this->n = n;
    }


    void SubtourElimination::callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                // Found an integer feasible solution - does it visit every node?
                auto **x = new double *[n];
                int *tour = new int[n];
                int i, j, len;
                for (i = 0; (std::size_t) i < n; i++)
                    x[i] = getSolution(vars[i], (int) n);

                tsp::findsubtour((int) n, x, &len, tour);

                if ((std::size_t) len < n) {
                    // Add subtour elimination constraint
                    GRBLinExpr expr = 0;
                    for (i = 0; i < len; i++) {
                        for (j = i + 1; j < len; j++) {
                            expr += vars[tour[i]][tour[j]];
                        }
                    }

                    addLazy(expr <= len - 1);
                }

                for (i = 0; (std::size_t) i < n; i++)
                    delete[] x[i];
                delete[] x;
                delete[] tour;
            }
        } catch (GRBException &e) {
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during callback" << std::endl;
        }
    }

    void findsubtour(int n, double **sol, int *tourlenP, int *tour) {
        bool *seen = new bool[n];
        int bestind, bestlen;
        int i, node, len, start;

        for (i = 0; i < n; i++)
            seen[i] = false;

        start = 0;
        bestlen = n + 1;
        bestind = -1;
        node = 0;
        while (start < n) {
            for (node = 0; node < n; node++)
                if (!seen[node])
                    break;
            if (node == n)
                break;
            for (len = 0; len < n; len++) {
                tour[start + len] = node;
                seen[node] = true;
                for (i = 0; i < n; i++) {
                    if (sol[node][i] > 0.5 && !seen[i]) {
                        node = i;
                        break;
                    }
                }
                if (i == n) {
                    len++;
                    if (len < bestlen) {
                        bestlen = len;
                        bestind = start;
                    }
                    start += len;
                    break;
                }
            }
        }

        for (i = 0; i < bestlen; i++)
            tour[i] = tour[bestind + i];
        *tourlenP = bestlen;

        delete[] seen;
    }

    TSPSolver::TSPSolver(const std::vector<Point> &witnesses,
                         const std::vector<Point> &tour_with_intersections, double time)
            :
            time(time),
            env(new GRBEnv()),
            model(GRBModel(*env)) {

        try {
            auto extended_tour = std::vector<Point>(tour_with_intersections);
            for (auto &witness: witnesses) {
                auto insert_pos = extended_tour.begin() + 1;
                auto prev_pos = extended_tour.begin();

                std::shared_ptr<Segment> shortest_dist_segment = nullptr;

                for (auto it = extended_tour.begin(); it != extended_tour.end(); it++) {
                    auto next = it + 1;
                    if (next == extended_tour.end()) next = extended_tour.begin();

                    if (witness == *it || witness == *next) {
                        shortest_dist_segment = nullptr;
                        break;
                    }

                    auto segment = ::utils::minimum_distance(*it, *next, witness);

                    if (shortest_dist_segment == nullptr ||
                        segment->squared_length() < shortest_dist_segment->squared_length()) {
                        shortest_dist_segment = segment;
                        insert_pos = next;
                        prev_pos = it;
                    }
                }

                if (shortest_dist_segment != nullptr &&
                    shortest_dist_segment->target() != *insert_pos &&
                    shortest_dist_segment->target() != *prev_pos) {
                    extended_tour.insert(insert_pos, shortest_dist_segment->target());
                }
            }

            for (auto it = extended_tour.begin(); it != extended_tour.end();) {
                auto next = it + 1;
                if (next == extended_tour.end()) next = extended_tour.begin();

                if (*it == *next) {
                    it = extended_tour.erase(next);
                } else {
                    it++;
                }
            }


            this->points = extended_tour;
            this->n = this->points.size();

            this->initialize_ip();

        } catch (GRBException &e) {
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during initialization" << std::endl;
        }


    }

    TSPSolver::TSPSolver(std::vector<Point> &points, double time)
            : time(time),
              env(new GRBEnv()),
              model(GRBModel(*env)) {

        try {
            this->points = std::vector<Point>(points);
            this->n = this->points.size();

            this->initialize_ip();

        } catch (GRBException &e) {
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during initialization" << std::endl;
        }
    }

    void TSPSolver::initialize_ip() {
        try {
            // Must set LazyConstraints parameter when using lazy constraints
            this->model.set(GRB_IntParam_LazyConstraints, 1);
            this->model.set(GRB_DoubleParam_TimeLimit, time);


            this->vars = new GRBVar *[this->n];
            for (std::size_t i = 0; i < n; i++) {
                this->vars[i] = new GRBVar[this->n];
            }

            for (std::size_t i = 0; i < this->n; i++) {
                for (std::size_t j = 0; j <= i; j++) {
                    vars[i][j] = model.addVar(0.0, 1.0,
                                              CGAL::to_double(
                                                      CGAL::approximate_sqrt(CGAL::squared_distance(this->points[i],
                                                                                                    this->points[j]))),
                                              GRB_BINARY);
                    vars[j][i] = vars[i][j];
                }
            }


            for (std::size_t i = 0; i < this->n; i++) {
                GRBLinExpr expr = 0;
                for (std::size_t j = 0; j < this->n; j++) {
                    expr += this->vars[i][j];
                }
                this->model.addConstr(expr == 2);
            }

            // Forbid edge from node back to itself
            for (std::size_t i = 0; i < n; i++) {
                this->vars[i][i].set(GRB_DoubleAttr_UB, 0);
            }

            auto cb = new SubtourElimination(vars, n);
            model.setCallback(cb);

            auto christofies_solver = Christofides(this->points);
            auto start_solution = christofies_solver.solve();

            for (std::size_t i = 0; i < this->n; i++) {
                for (std::size_t j = 0; j <= i; j++) {
                    this->vars[i][j].set(GRB_DoubleAttr_Start, 0);
                }
            }
            for (auto it = start_solution.begin(); it != start_solution.end(); it++) {
                auto next = it + 1;
                if (next == start_solution.end()) next = start_solution.begin();
                this->vars[*it][*next].set(GRB_DoubleAttr_Start, 1.0);
            }

        } catch (GRBException &e) {
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during initialization" << std::endl;
        }
    }

    TSPSolver::solution TSPSolver::solve() {
        auto result = solution{std::vector<Point>(),
                               std::vector<std::size_t>(), 0, 0, 0, this->time * 1000, false};


        using clock = std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using time_unit = std::chrono::milliseconds;

        auto start = clock::now();

        try {
            this->model.optimize();
        } catch (GRBException &e) {
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during optimization" << std::endl;
        }
        result.solved_optimally = this->model.get(GRB_IntAttr_Status) == GRB_OPTIMAL;
        result.time = duration_cast<time_unit>(clock::now() - start).count();
        result.lower_bound = this->model.get(GRB_DoubleAttr_ObjBound);
        result.upper_bound = this->model.get(GRB_DoubleAttr_ObjVal);

        auto **sol = new double *[n];

        for (std::size_t i = 0; i < this->n; i++) {
            sol[i] = this->model.get(GRB_DoubleAttr_X, vars[i], n);
        }

        int *tour = new int[n];
        int len;
        findsubtour(n, sol, &len, tour);

        for (std::size_t i = 0; i < (std::size_t) len; i++) {
            result.indices.emplace_back(i);
            result.points.emplace_back(this->points[tour[i]]);
            delete[] sol[i];
        }

        delete[] sol;
        delete[] tour;

        return result;
    }
}

