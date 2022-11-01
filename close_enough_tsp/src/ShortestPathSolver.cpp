#include "close_enough_tsp/ShortestPathSolver.h"

ShortestPathSolver::ShortestPathSolver(Point &start_point, Point &end_point,
                                       std::vector<Point> &points,
                                       double radius,
                                       double time, double ub) : points(points) {
    this->radii = std::vector<double>();
    this->demands = std::vector<double>();

    for (auto it = this->points.begin(); it != this->points.end();) {
        bool isUnique = true;
        for (auto it2 = it + 1; it2 != this->points.end(); it2++) {
            if (*it == *it2) {
                isUnique = false;
                break;
            }
        }

        if (isUnique) {
            it++;
        } else {
            it = this->points.erase(it);
        }
    }

    for ([[maybe_unused]] auto &p: this->points) {
        this->radii.emplace_back(radius);
        this->demands.emplace_back(1.0);
    }

    this->points.insert(this->points.begin(), start_point);
    this->points.emplace_back(end_point);

    this->radii.insert(this->radii.begin(), 0);
    this->radii.emplace_back(0);
    this->demands.insert(this->demands.begin(), 0);
    this->demands.emplace_back(0);

    this->time = time;
    this->initial_upper_bound = ub;
}

ShortestPathSolver::solution ShortestPathSolver::solve() {

    [[maybe_unused]] int selectingRoot = ROOT_SELECTION_1;
    int branchingRule = BRANCHING_RULE_SB;
    int branchingStrategy = BRANCHING_STRATEGY_DFS;
    int strong_branching_size = 2; // FIXME

    double timeLimit = this->time;
    double overlap = 1.0;

    double somaTeste = 0;


    Data *dataptr = new Data(this->points,
                             this->radii,
                             this->demands,
                             overlap);

    int sizeInst = dataptr->getSizeInst();
    cout << "Instance Size " << sizeInst << endl;
    cout << fixed;

    //starting the branch and bound
    double initialTotalTimeBnB = cpuTime();
    list<node *> open;
    list<node *>::iterator itOpen;

    node *root = new node;

    double bestKnown = this->initial_upper_bound;
    cout << "Initial Upper Bound: " << bestKnown << endl;
    double ub = bestKnown + 1;
    double best = DBL_MAX;
    double best_lb = DBL_MAX;
    double best_ub = DBL_MAX;
    unsigned long int count_SOCP_solved = 0;
    double rootLB = 0;

    // int precision = 4;
    int printHeader = 0;
    bool optimalFound = true;
    int print_counter = floor(sizeInst / 2);

    auto *bnbPtr = new BranchNBound(dataptr);

    // Selecting initial root
    root->pts = {0, 1, (int) this->points.size()-1};

    cout << "Initial root: ";
    for (int pt : root->pts) {
        cout << pt << " ";
    }
    cout << endl;
    //####################################

    auto *solveSocpPtr = new SolveSocpCplex(dataptr, root->pts);
    //solve model
    double totalSocpCompTime = 0;
    double initialSocpCompTime = cpuTime();
    solveSocpPtr->solveSOCP(root->pts);
    double finalSocpCompTime = cpuTime();
    totalSocpCompTime += (finalSocpCompTime - initialSocpCompTime);
    count_SOCP_solved++;

    root->lb = solveSocpPtr->getF_value();
    rootLB = root->lb;
    root->s_lb = 1;
    solveSocpPtr->printF_value();
    solveSocpPtr->finishSOCP();
    solveSocpPtr->printSolution(root->pts);
    somaTeste += solveSocpPtr->violation;

    //get solution
    vector<vector<double>> solutionXYZ;
    vector<double> tempX;
    vector<double> tempY;
    vector<double> tempZ;

    for (std::size_t i = 0; i < root->pts.size(); i++) {
        tempX.push_back(solveSocpPtr->getSolutionX(i));
        tempY.push_back(solveSocpPtr->getSolutionY(i));
        tempZ.push_back(solveSocpPtr->getSolutionZ(i));
    }

    solutionXYZ.push_back(tempX);
    solutionXYZ.push_back(tempY);
    solutionXYZ.push_back(tempZ);

    //check feasibility
    bool feasibilityTest;
    feasibilityTest = bnbPtr->check_feasibility_Q(root->pts, solutionXYZ);

    cout << endl;
    cout << "Not covered at the root ";
    for (int notCoveredBall : bnbPtr->notCoveredBalls) {
        cout << notCoveredBall << " ";
    }
    cout << endl;
    //check not covered clients
    for (int &notCoveredBall : bnbPtr->notCoveredBalls) {
        root->notCovered.push_back(notCoveredBall);
    }

    if (feasibilityTest) {
        cout << "FEASIBLE ROOT" << endl;
        best_lb = rootLB;
        best = rootLB;
        best_ub = rootLB;
        // double finalTotalTimeBnB = cpuTime();
        // double computationTime = finalTotalTimeBnB - initialTotalTimeBnB;
        // double gap_root = ((ub - rootLB) / ub) * 100;
        //printDataToFile(dataptr, overlap, sizeInst, bestKnown, best, best_lb, 0, count_SOCP_solved, 0,
        //                computationTime, 0, computationTime);

        std::cout << "Feasible start solution " << std::endl;

        return solution{best_lb, best_ub, this->convertSolution(solutionXYZ), true};
    } else {
        cout << "INFEASIBLE ROOT" << endl;
    }
    cout << endl;

    solutionXYZ.clear();
    tempX.clear();
    tempY.clear();
    tempZ.clear();

    open.push_back(root);
    vector<int> solutionIndices;
    vector<vector<double>> solutionCoordinatesXYZ;
    int itCount = 0;
    int k = 0;
    double sbComputationTime = 0;

    //set branching rule
    long strongBranchingSize = 0;
    if (branchingRule == BRANCHING_RULE_V1) {
        strongBranchingSize = 1;
    } else if (branchingRule == BRANCHING_RULE_SB) {
        strongBranchingSize = strong_branching_size;
    } else {

    }

    vector<sbAuxStruct *> vectorOfChildren;
    list<int>::iterator stBrchit;

    while (!open.empty() && cpuTime() - initialTotalTimeBnB <= timeLimit) {

        node *current;
        //######### Depth First Search ###########
        if (branchingStrategy == BRANCHING_STRATEGY_DFS) {
            current = open.back();
            open.pop_back();
        }
        //######### Depth First Search ###########

        //######### Breadth First Search ###########
        if (branchingStrategy == BRANCHING_STRATEGY_BFS) {
            current = open.front();
            open.pop_front();
        }
        //######### Breadth First Search ###########

        //######### Best First Search ###########
        if (branchingStrategy == BRANCHING_STRATEGY_BeFS) {
            for (itOpen = open.begin(); itOpen != open.end(); itOpen++) {
                if ((*itOpen)->s_lb == 1) {
                    current = (*itOpen);
                }
            }
            open.remove(current);
            if (!open.empty()) {
                bnbPtr->computeLowerBounds(&open, current, &best_lb);
            }
        }
        //######### Best First Search ###########

        if (current->notCovered.empty() || current->lb > ub) {
            // FATHOMED BY BOUND
            delete current;
        } else {
            stBrchit = current->notCovered.begin();

            //control strong branching size
            if (branchingRule == BRANCHING_RULE_SB) {
                strongBranchingSize = strong_branching_size;
                strongBranchingSize -= (long) current->pts.size() - 3;
            }
            if (strongBranchingSize < 1) {
                strongBranchingSize = 1;
            }
            //control strong branching size

            //begin strong branching
            double initialTimeSB = cpuTime();
            if (strongBranchingSize > 1) {
                cout << "Doing Strong Branching: " << strongBranchingSize << endl;
            }
            for (std::size_t t = 0; t < (std::size_t) strongBranchingSize && t < current->notCovered.size(); t++) {
                k = (*stBrchit);
                stBrchit++;
                auto *tempSbStr = new sbAuxStruct;
                tempSbStr->index = k;
                tempSbStr->sum = 0;

                for (std::size_t i = 0; i < current->pts.size()-1; i++) {

                    node *child = new node;
                    child->s_lb = 0;
                    child->pts = bnbPtr->insert(current->pts, i, k);

                    auto *solveSocpPtr2 = new SolveSocpCplex(dataptr, child->pts);

                    initialSocpCompTime = cpuTime();
                    solveSocpPtr2->solveSOCP(child->pts);
                    finalSocpCompTime = cpuTime();
                    totalSocpCompTime += (finalSocpCompTime - initialSocpCompTime);

                    child->lb = solveSocpPtr2->getF_value();
                    solveSocpPtr2->finishSOCP();
                    somaTeste += solveSocpPtr2->violation;

                    itCount++;

                    for (std::size_t i2 = 0; i2 < child->pts.size(); i2++) {
                        tempX.push_back(solveSocpPtr2->getSolutionX(i2));
                        tempY.push_back(solveSocpPtr2->getSolutionY(i2));
                        tempZ.push_back(solveSocpPtr2->getSolutionZ(i2));
                    }

                    child->solXYZ.push_back(tempX);
                    child->solXYZ.push_back(tempY);
                    child->solXYZ.push_back(tempZ);

                    //check feasibility
                    feasibilityTest = bnbPtr->check_feasibility_Q(child->pts, child->solXYZ);

                    if (feasibilityTest) {
                        child->feasible = 1;
                    } else {
                        child->feasible = 0;
                    }
                    tempX.clear();
                    tempY.clear();
                    tempZ.clear();

                    child->notCovered.clear();

                    //check not covered clients
                    for (int &notCoveredBall : bnbPtr->notCoveredBalls) {
                        child->notCovered.push_back(notCoveredBall);
                    }
                    //
                    if (child->lb < best_ub && child->lb >= ub) {
                        if (feasibilityTest) best_ub = child->lb;
                    }

                    //fill the vector of children
                    if (child->lb < ub) {
                        tempSbStr->sum += child->lb;
                    } else {
                        tempSbStr->sum += ub;
                    }

                    tempSbStr->candidates.push_back(child);
                    delete solveSocpPtr2;
                }
                vectorOfChildren.push_back(tempSbStr);
            }

            double finalTimeSB = cpuTime();
            sbComputationTime += finalTimeSB - initialTimeSB;

            //take the best sum
            double bestSum = 0;
            std::size_t pos = 0;
            for (std::size_t s0 = 0; s0 < vectorOfChildren.size(); s0++) {
                if (vectorOfChildren[s0]->sum > bestSum) {
                    bestSum = vectorOfChildren[s0]->sum;
                    pos = s0;
                }
            }

            //get upper bounds
            for (auto &s0 : vectorOfChildren) {
                for (auto &candidate : s0->candidates) {
                    if (candidate->lb < ub &&
                        candidate->feasible == 1)
                        ub = candidate->lb;
                    if (candidate->lb < best &&
                        candidate->feasible == 1) {
                        best = candidate->lb;
                        solutionIndices = candidate->pts;
                        solutionCoordinatesXYZ = candidate->solXYZ;
                    }
                }
            }

            for (std::size_t s0 = 0; s0 < vectorOfChildren.size(); s0++) {
                if (s0 != pos) {
                    for (auto &candidate : vectorOfChildren[s0]->candidates) {
                        delete candidate;
                    }
                    delete vectorOfChildren[s0];
                }
            }

            //select candidates for branching
            for (auto &candidate : vectorOfChildren[pos]->candidates) {
                count_SOCP_solved++;
                if (candidate->lb <= ub) {
                    if (candidate->feasible == 1) {
                        ub = candidate->lb;
                        if (candidate->lb < best) {
                            best = candidate->lb;
                            solutionIndices = candidate->pts;
                            solutionCoordinatesXYZ = candidate->solXYZ;
                        }

                        for (itOpen = open.begin(); itOpen != open.end(); itOpen++) {
                            double lowerB = (*itOpen)->lb;
                            if (lowerB > ub) {
                                open.erase(itOpen);
                                itOpen = open.begin();
                            }
                        }
                        delete candidate;
                        candidate = nullptr;
                    } else {
                        open.push_back(candidate);
                        //print log
                        if (print_counter == floor(sizeInst / 2)) {
                            bnbPtr->printLog(candidate, open,
                                             count_SOCP_solved, best_ub, best_lb,
                                             candidate->notCovered.size(), &printHeader);
                            print_counter = 0;
                        }
                        print_counter++;
                    }
                } else {
                    delete candidate;
                    candidate = nullptr;
                }
            }

            //COMPUTE LOWER BOUNDS
            //---------------------------------------------------------------------
            if (current->s_lb == 1 && !open.empty()) {
                bnbPtr->computeLowerBounds(&open, current, &best_lb);
            }
            //---------------------------------------------------------------------

            vectorOfChildren.clear();
        }
        if (cpuTime() - initialTotalTimeBnB > timeLimit) optimalFound = false;

    }

    double finalTotalTimeBnB = cpuTime();
    double computationTime = finalTotalTimeBnB - initialTotalTimeBnB;

    // double gap_root = ((ub - rootLB) / ub) * 100;
    // double gap_real = ((bestKnown - best_lb) / bestKnown) * 100;
    // double gap_lb_bnb = ((ub - best_lb) / ub) * 100;

    auto converted_solution = this->convertSolution(solutionCoordinatesXYZ);

    // Fixing incorrect upper bounds
    {
        Kernel::FT tourLength = 0.0;

        for (auto it = converted_solution.begin(); it != converted_solution.end(); it++) {

            auto next = it + 1;

            if (next == converted_solution.end()) next = converted_solution.begin();

            tourLength += CGAL::approximate_sqrt(CGAL::squared_distance(*it, *next));
        }

        best_ub = CGAL::to_double(tourLength);

        if (optimalFound && best_lb > best_ub + 0.0001) {
            best_lb = best_ub; // FIXME: This is a dirty hack
        }
    }

    //Finish Branch and Bound

    cout << endl;
    cout << "### Final Log ###" << endl << endl;
    if (optimalFound) {
        cout << "OPTIMAL SOLUTION FOUND" << endl;
        cout << "Function objective value: " << best << endl;
        cout << "Sum of unviability: " << somaTeste << endl;
        cout << "Sequence: ";
        for (int i : solutionIndices) {
            cout << i << " ";
        }
        cout << endl;

        //printDataToFile(dataptr, overlap, sizeInst, bestKnown, best, best_lb, gap_root, count_SOCP_solved,
        //                itCount, computationTime, sbComputationTime, totalSocpCompTime);

        //printDataToMatlab(dataptr, sizeInst, overlap, best, solutionIndices, solutionCoordinatesXYZ);
    } else {

        cout << "NO OPTIMAL SOLUTION FOUND" << endl;
        cout << "Lower Bound: " << best_lb << endl;
        cout << "Upper Bound: " << ub << endl;
        cout << "GAP(LB): " << ((ub - best_lb) / ub) * 100 << "% " << endl;

        //printDataToFile(dataptr, overlap, sizeInst, bestKnown, ub, best_lb, gap_real, gap_lb_bnb, gap_root,
        //                count_SOCP_solved, itCount, computationTime, sbComputationTime, totalSocpCompTime);
    }

    cout << "Number of Nodes Resolved: " << count_SOCP_solved << endl;
    /*cout << "Percentage of tree pruned: " << temp << "%" << endl;
    cout << "Percentage of the computed tree: " << prctComp << "%" << endl;
    cout << "Total: " << temp + prctComp << endl;*/
    cout << "Time total: " << computationTime << endl;
    cout << "Time S.B: " << sbComputationTime << endl;
    cout << "Time SOCP: " << totalSocpCompTime << endl;

    cout << endl << "#################" << endl;
    delete root;
    delete bnbPtr;
    delete solveSocpPtr;
    delete dataptr;


    return solution{best_lb, best_ub, this->convertSolution(solutionCoordinatesXYZ), optimalFound};
}

std::vector<Point> ShortestPathSolver::convertSolution(std::vector<std::vector<double>> &solution) {
    auto return_points = std::vector<Point>();

    if (!solution.empty()) {
        std::size_t tour_length = solution[0].size();
        for (std::size_t i = 0; i < tour_length; i++) {
            //std::cout << solution[0][i] << " " << solution[1][i] << std::endl;
            return_points.emplace_back(solution[0][i], solution[1][i]);
        }
    }

    return return_points;
}



















