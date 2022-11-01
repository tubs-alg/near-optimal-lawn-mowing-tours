#include "close_enough_tsp/BranchNBound.h"

BranchNBound::BranchNBound(Data *dataObject)
        : objectOfData(dataObject) {
    sizeOfInstance = objectOfData->getSizeInst();

    //select branching rule
    branching_rule = 0;
    for (int i = 2; i < sizeOfInstance; i++) {
        if (objectOfData->getRadius(i) != objectOfData->getRadius(i - 1)) {
            branching_rule = 2;
            cout << "Using branching rule #2!" << endl;
            break;
        } else {
            branching_rule = 1;
        }
    }
    if (branching_rule == 1) {
        cout << "Using branching rule #1!" << endl;
    }

}

// destructor
BranchNBound::~BranchNBound() = default;

int BranchNBound::getNotCoveredBalls(int i) {
    return notCoveredBalls[i];
}

double norm(vector<double> element) {
    double normOfElement = 0;
    normOfElement = sqrt(pow(element[0], 2) + pow(element[1], 2) + pow(element[2], 2));
    return normOfElement;
}

bool sortVectorOfPairs(pair<int, double> i, pair<int, double> j) {
    return (i.second > j.second);
}

vector<int> BranchNBound::selectRoot() {
    //	escolher os elementos que entram na raiz
    double greatestSolution = 0;
    double temp = 0;
    vector<int> tempSequence;
    vector<int> sequence;
    tempSequence.resize(3);
    sequence.resize(3);

    int sizeInst = objectOfData->getSizeInst();

    sequence[0] = 0;
    sequence[1] = objectOfData->getDepotFarthest(0);

    tempSequence[0] = 0;
    tempSequence[1] = objectOfData->getDepotFarthest(0);

    for (int i = 1; i < sizeInst; i++) {
        tempSequence[2] = i;
        auto solveCplexSGR = new SolveSocpCplex(objectOfData, tempSequence);
        solveCplexSGR->solveSOCP(tempSequence);
        temp = solveCplexSGR->getF_value();
        if (temp > greatestSolution) {
            greatestSolution = temp;
            sequence[2] = i;
        }
        delete solveCplexSGR;
    }

    cout << "Raiz: ";
    for (int i = 0; i < 3; i++) {
        cout << sequence[i] << " ";
    }
    cout << endl;

    return sequence;

}

vector<int> BranchNBound::selectRoot2() {
    //escolher os elementos que entram na raiz
    double greatestSolution = 0;
    double temp = 0;
    vector<int> tempSequence;
    vector<int> sequence;
    tempSequence.resize(3);
    sequence.resize(3);

    int sizeInst = objectOfData->getSizeInst();

    for (int i = 0; i < sizeInst; i++) {
        for (int j = i + 1; j < sizeInst; j++) {
            for (int k = j + 1; k < sizeInst; k++) {
                tempSequence[0] = i;
                tempSequence[1] = j;
                tempSequence[2] = k;
                auto solveCplexSGR = new SolveSocpCplex(objectOfData, tempSequence);
                solveCplexSGR->solveSOCP(tempSequence);
                temp = solveCplexSGR->getF_value();
                if (temp > greatestSolution) {
                    greatestSolution = temp;
                    sequence = tempSequence;
                }

                solveCplexSGR->finishSOCP();
                delete solveCplexSGR;

                if (cpuTime() > 1800) {
                    return sequence;
                }
            }
        }
    }

    cout << "Raiz: ";
    for (int i = 0; i < 3; i++) {
        cout << sequence[i] << " ";
    }
    cout << endl;

    return sequence;

}

vector<int> BranchNBound::selectRoot3() {
    //escolher os elementos que entram na raiz
    vector<int> tempSequence;
    vector<int> sequence;
    tempSequence.resize(3);
    sequence.resize(3);
    vector<vector<double>> solutionXYZ;
    vector<double> tempX;
    vector<double> tempY;
    vector<double> tempZ;

    int notCoveredNumber = INT_MAX;
    int sizeInst = objectOfData->getSizeInst();

    for (int i = 0; i < sizeInst; i++) {
        for (int j = i + 1; j < sizeInst; j++) {
            for (int k = j + 1; k < sizeInst; k++) {
                tempSequence[0] = i;
                tempSequence[1] = j;
                tempSequence[2] = k;
                auto *solveCplexSGR3 = new SolveSocpCplex(objectOfData, tempSequence);
                solveCplexSGR3->solveSOCP(tempSequence);
                //temp = solveCplexSGR->getF_value();

                //get solution
                for (std::size_t l = 0; l < tempSequence.size(); l++) {
                    tempX.push_back(solveCplexSGR3->getSolutionX(l));
                    tempY.push_back(solveCplexSGR3->getSolutionY(l));
                    tempZ.push_back(solveCplexSGR3->getSolutionZ(l));
                }
                solutionXYZ.push_back(tempX);
                solutionXYZ.push_back(tempY);
                solutionXYZ.push_back(tempZ);
                //end get solution

                auto BnB = new BranchNBound(objectOfData);
                [[maybe_unused]] bool feasibilityTest = BnB->check_feasibility_Q(tempSequence, solutionXYZ);
                //check not covered clients
                if (BnB->notCoveredBalls.size() < (unsigned int) notCoveredNumber) {
                    notCoveredNumber = BnB->notCoveredBalls.size();
                    sequence = tempSequence;
                }

                solveCplexSGR3->finishSOCP();
                solutionXYZ.clear();
                tempX.clear();
                tempY.clear();
                tempZ.clear();

                delete solveCplexSGR3;
                delete BnB;

                if (cpuTime() > 1800) {
                    return sequence;
                }
            }
        }
    }

    cout << "Raiz: ";
    for (int i = 0; i < 3; i++) {
        cout << sequence[i] << " ";
    }
    cout << endl;

    return sequence;

}

bool BranchNBound::checkFeasibility(vector<vector<double>> solution, [[maybe_unused]] vector<int> sequence) {
    int sizeSequence = 0;
    int feasibility = 1;

    sizeSequence = solution[0].size() - 1;

    vector<int> coveredBalls;
    coveredBalls.resize(sizeOfInstance);

    vector<double> r;
    r.resize(3);
    vector<double> p1v;
    p1v.resize(3);
    vector<double> p2v;
    p2v.resize(3);
    vector<double> v;
    v.resize(3);
    vector<double> cv;
    cv.resize(3);
    vector<double> cp1;
    cp1.resize(3);
    vector<double> cp2;
    cp2.resize(3);

    double mr = 0;
    double norm_cv = 0;
    double test = 0;

    notCoveredBalls.clear();

    for (int j = 0; j < sizeOfInstance; j++) {
        for (int i = 0; i < sizeSequence; i++) {

            cp1[0] = solution[0][i] - objectOfData->getCoordx(j);
            cp1[1] = solution[1][i] - objectOfData->getCoordy(j);

            cp2[0] = solution[0][i + 1] - objectOfData->getCoordx(j);
            cp2[1] = solution[1][i + 1] - objectOfData->getCoordy(j);

            if (norm(cp1) <= objectOfData->getRadius(j) + 0.005 * objectOfData->getRadius(j) ||
                norm(cp2) <= objectOfData->getRadius(j) + 0.005 * objectOfData->getRadius(j)) {
                coveredBalls[j] = 1;
                i = sizeSequence;
            } else {
                r[0] = solution[0][i + 1] - solution[0][i];
                r[1] = solution[1][i + 1] - solution[1][i];

                mr = r[1] / r[0];

                v[0] = (mr * (mr * solution[0][i] + solution[1][i] - objectOfData->getCoordy(j) +
                              objectOfData->getCoordx(j))) / (1 - mr * mr);
                v[1] = objectOfData->getCoordy(j) + (1 / mr) * (v[0] - objectOfData->getCoordx(j));

                cv[0] = v[0] - objectOfData->getCoordx(j);
                cv[1] = v[1] - objectOfData->getCoordy(j);

                norm_cv = 0;
                norm_cv = norm(cv);

                p1v[0] = v[0] - solution[0][i];
                p1v[1] = v[1] - solution[1][i];
                p1v[2] = v[2] - solution[2][i];

                p2v[0] = v[0] - solution[0][i + 1];
                p2v[1] = v[1] - solution[1][i + 1];
                p2v[2] = v[2] - solution[2][i + 1];

                test = norm(p1v);

                if (norm(p2v) > test) {
                    test = norm(p2v);
                }

                if (norm_cv <= objectOfData->getRadius(j)) {
                    if (test <= norm(r)) {
                        coveredBalls[j] = 1;
                        i = sizeSequence;
                    } else {
                    }
                } else {
                }
            }
        }

        cp1[0] = solution[0][sizeSequence] - objectOfData->getCoordx(j);
        cp1[1] = solution[1][sizeSequence] - objectOfData->getCoordy(j);

        cp2[0] = solution[0][0] - objectOfData->getCoordx(j);
        cp2[1] = solution[1][0] - objectOfData->getCoordy(j);

        if (norm(cp1) <= objectOfData->getRadius(j) + 0.1 * objectOfData->getRadius(j) ||
            norm(cp2) <= objectOfData->getRadius(j) + 0.1 * objectOfData->getRadius(j)) {
            coveredBalls[j] = 1;
        } else {

            r[0] = solution[0][0] - solution[0][sizeSequence];
            r[1] = solution[1][0] - solution[1][sizeSequence];
            r[2] = solution[2][0] - solution[2][sizeSequence];

            mr = r[1] / r[0];

            v[0] = (mr * (mr * solution[0][sizeSequence] + solution[1][sizeSequence] - objectOfData->getCoordy(j) +
                          objectOfData->getCoordx(j))) / (1 - mr * mr);
            v[1] = objectOfData->getCoordy(j) + (1 / mr) * (v[0] - objectOfData->getCoordx(j));

            cv[0] = v[0] - objectOfData->getCoordx(j);
            cv[1] = v[1] - objectOfData->getCoordy(j);

            norm_cv = norm(cv);

            p1v[0] = v[0] - solution[0][sizeSequence];
            p1v[1] = v[1] - solution[1][sizeSequence];
            p1v[2] = v[2] - solution[2][sizeSequence];

            p2v[0] = v[0] - solution[0][0];
            p2v[1] = v[1] - solution[1][0];
            p2v[2] = v[2] - solution[2][0];

            test = norm(p1v);

            if (norm(p2v) > test) {
                test = norm(p2v);
            }

            if (norm_cv <= objectOfData->getRadius(j)) {
                if (test <= norm(r)) {
                    coveredBalls[j] = 1;
                } else {
                }
            } else {
            }
        }
    }

    for (int i = 0; i < sizeOfInstance; i++) {
        feasibility *= coveredBalls[i];
    }

    for (std::size_t i = 0; i < coveredBalls.size(); i++) {
        if (coveredBalls[i] != 1) {
            notCoveredBalls.push_back(i);
        }
    }

    if (feasibility == 1) {
        return true;
    } else {
        return false;
    }
}

double euclidianNorm(point *p1, point *p2) {
    return sqrt(pow(p2->x - p1->x, 2) + pow(p2->y - p1->y, 2) + pow(p2->z - p1->z, 2));
}

double Norm_2(vector<double> aVector) {
    return sqrt(pow(aVector[0], 2) + pow(aVector[1], 2) + pow(aVector[2], 2));
}

vector<double> make_vector(point *p1, point *p2) {
    vector<double> aVector;

    aVector.push_back(p2->x - p1->x);
    aVector.push_back(p2->y - p1->y);
    aVector.push_back(p2->z - p1->z);

    return aVector;
}

double dot_product(vector<double> vector1, vector<double> vector2) {
    return (vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2]);
}

vector<double> cross_product(vector<double> vector1, vector<double> vector2) {
    vector<double> crossProduct;
    crossProduct.resize(3);

    crossProduct[0] = vector2[1] * vector1[2] - vector2[2] * vector1[1];
    crossProduct[1] = vector2[2] * vector1[0] - vector2[0] * vector1[2];
    crossProduct[2] = vector2[0] * vector1[1] - vector2[1] * vector1[0];

    return crossProduct;
}

vector<double> difference(vector<double> vector1, vector<double> vector2) {
    vector<double> dif;
    dif.resize(3);

    dif[0] = vector2[0] - vector1[0];
    dif[1] = vector2[1] - vector1[1];
    dif[2] = vector2[2] - vector1[2];

    return dif;
}

vector<double> sum_vector(vector<double> vector1, vector<double> vector2) {
    vector<double> sum;
    sum.resize(3);

    sum[0] = vector2[0] + vector1[0];
    sum[1] = vector2[1] + vector1[1];
    sum[2] = vector2[2] + vector1[2];

    return sum;
}

vector<double> scalar_product(double lambda, vector<double> vector1) {
    vector<double> prod;
    prod.resize(3);

    prod[0] = lambda * vector1[0];
    prod[1] = lambda * vector1[1];
    prod[2] = lambda * vector1[2];

    return prod;
}


bool BranchNBound::check_feasibility_Q([[maybe_unused]] vector<int> sequence, vector<vector<double>> solution) {
    vector<double> c(3);
    vector<double> p1(3);
    vector<double> p2(3);
    vector<double> point(3);

    double constant = 0.0001;
    long double theta = 0;

    vector<vector<double>> solutionTemp = solution;
    solutionTemp[0].push_back(solution[0].front());
    solutionTemp[1].push_back(solution[1].front());
    solutionTemp[2].push_back(solution[2].front());

    vector<int> coveredBalls(sizeOfInstance);

    for (int i = 0; i < sizeOfInstance; i++) {
        for (unsigned int j = 0; j < solutionTemp[0].size() - 1; j++) {

            c[0] = objectOfData->getCoordx(i);
            c[1] = objectOfData->getCoordy(i);
            c[2] = objectOfData->getCoordz(i);

            p1[0] = solutionTemp[0][j];
            p1[1] = solutionTemp[1][j];
            p1[2] = solutionTemp[2][j];

            p2[0] = solutionTemp[0][j + 1];
            p2[1] = solutionTemp[1][j + 1];
            p2[2] = solutionTemp[2][j + 1];

            theta = -(dot_product(difference(p2, p1), difference(c, p2))) / (pow(Norm_2(difference(p2, p1)), 2));
            if (theta >= 1) {
                theta = 1;
                point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                double test = Norm_2(difference(point, c));
                if (test < objectOfData->getRadius(i) + constant) {
                    coveredBalls[i] = 1;
                }
            } else {
                if (theta > 0 && theta < 1) {
                    point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                    double test = Norm_2(difference(point, c));
                    if (test < objectOfData->getRadius(i) + constant) {
                        coveredBalls[i] = 1;
                    }
                } else {
                    if (theta <= 0) {
                        theta = 0;
                        point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                        double test = Norm_2(difference(point, c));
                        if (test < objectOfData->getRadius(i) + constant) {
                            coveredBalls[i] = 1;
                        }
                    }
                }
            }
        }
    }

    double feasibility = 1;

    for (int i = 0; i < sizeOfInstance; i++) {
        feasibility *= coveredBalls[i];
    }

    notCoveredBalls.clear();
    for (std::size_t i = 0; i < coveredBalls.size(); i++) {
        if (coveredBalls[i] != 1) {
            notCoveredBalls.push_back(i);
        }
    }

    if (branching_rule == 1) {
        sortNotCovered(notCoveredBalls, solutionTemp);
    }
    if (branching_rule == 2) {
        sortNotCovered2(notCoveredBalls, solutionTemp);
    }

    if (feasibility == 1) {
        return true;
    } else {
        return false;
    }
}

void BranchNBound::sortNotCovered(vector<int> notCovered, vector<vector<double>> solutionTemp) {
    vector<pair<int, double>> aux;

    pair<int, double> temp;

    vector<double> c;
    vector<double> p1;
    vector<double> p2;
    vector<double> point;

    c.resize(3);
    p1.resize(3);
    p2.resize(3);

    double min = DBL_MAX;
    long double theta = 0;

    for (int & i : notCovered) {
        for (std::size_t j = 0; j < solutionTemp[0].size() - 1; j++) {

            c[0] = objectOfData->getCoordx(i);
            c[1] = objectOfData->getCoordy(i);
            c[2] = objectOfData->getCoordz(i);

            p1[0] = solutionTemp[0][j];
            p1[1] = solutionTemp[1][j];
            p1[2] = solutionTemp[2][j];

            p2[0] = solutionTemp[0][j + 1];
            p2[1] = solutionTemp[1][j + 1];
            p2[2] = solutionTemp[2][j + 1];

            theta = -(dot_product(difference(p2, p1), difference(c, p2))) / (pow(Norm_2(difference(p2, p1)), 2));
            if (theta >= 1) {
                theta = 1;
                point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                double test = Norm_2(difference(point, c));
                if (test < min) {
                    min = test;
                    temp = make_pair(i, min);
                }
            } else {
                if (theta > 0 && theta < 1) {
                    point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                    double test = Norm_2(difference(point, c));
                    if (test < min) {
                        min = test;
                        temp = make_pair(i, min);
                    }
                } else {
                    if (theta <= 0) {
                        theta = 0;
                        point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                        double test = Norm_2(difference(point, c));
                        if (test < min) {
                            min = test;
                            temp = make_pair(i, min);
                        }
                    }
                }
            }
        }
        min = DBL_MAX;
        aux.push_back(temp);
    }

    sort(aux.begin(), aux.end(), sortVectorOfPairs);

    for (std::size_t i = 0; i < notCovered.size(); i++) {
        notCoveredBalls[i] = aux[i].first;
    }
}

void BranchNBound::sortNotCovered2(vector<int> notCovered, vector<vector<double>> solutionTemp) {
    vector<pair<int, double>> aux;

    pair<int, double> temp;

    vector<double> c(3);
    vector<double> p1(3);
    vector<double> p2(3);
    vector<double> point(3);
    vector<double> point_in_the_border(3);

    double min = DBL_MAX;
    long double theta = 0;

    for (int & i : notCovered) {
        for (std::size_t j = 0; j < solutionTemp[0].size() - 1; j++) {

            c[0] = objectOfData->getCoordx(i);
            c[1] = objectOfData->getCoordy(i);
            c[2] = objectOfData->getCoordz(i);

            p1[0] = solutionTemp[0][j];
            p1[1] = solutionTemp[1][j];
            p1[2] = solutionTemp[2][j];

            p2[0] = solutionTemp[0][j + 1];
            p2[1] = solutionTemp[1][j + 1];
            p2[2] = solutionTemp[2][j + 1];

            theta = -(dot_product(difference(p2, p1), difference(c, p2))) / (pow(Norm_2(difference(p2, p1)), 2));
            if (theta >= 1) {
                theta = 1;
                point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                double test = Norm_2(difference(point, c));
                if (test < min) {
                    double lambda = objectOfData->getRadius(i) / test;
                    point_in_the_border = sum_vector(scalar_product(lambda, point), scalar_product(1 - lambda, c));
                    double side_one = Norm_2(difference(point_in_the_border, p1));
                    double side_two = Norm_2(difference(point_in_the_border, p2));
                    double base = Norm_2(difference(p2, p1));
                    double gamma = side_one + side_two - base;
                    min = test;
                    temp = make_pair(i, gamma);
                }
            } else {
                if (theta > 0 && theta < 1) {
                    point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                    double test = Norm_2(difference(point, c));
                    if (test < min) {
                        double lambda = objectOfData->getRadius(i) / test;
                        point_in_the_border = sum_vector(scalar_product(lambda, point), scalar_product(1 - lambda, c));
                        double side_one = Norm_2(difference(point_in_the_border, p1));
                        double side_two = Norm_2(difference(point_in_the_border, p2));
                        double base = Norm_2(difference(p2, p1));
                        double gamma = side_one + side_two - base;
                        min = test;
                        temp = make_pair(i, gamma);
                    }
                } else {
                    if (theta <= 0) {
                        theta = 0;
                        point = sum_vector(scalar_product(theta, p1), scalar_product(1 - theta, p2));
                        double test = Norm_2(difference(point, c));
                        if (test < min) {
                            double lambda = objectOfData->getRadius(i) / test;
                            point_in_the_border = sum_vector(scalar_product(lambda, point),
                                                             scalar_product(1 - lambda, c));
                            double side_one = Norm_2(difference(point_in_the_border, p1));
                            double side_two = Norm_2(difference(point_in_the_border, p2));
                            double base = Norm_2(difference(p2, p1));
                            double gamma = side_one + side_two - base;
                            min = test;
                            temp = make_pair(i, gamma);
                        }
                    }
                }
            }
        }
        min = DBL_MAX;
        aux.push_back(temp);
    }

    sort(aux.begin(), aux.end(), sortVectorOfPairs);

    for (std::size_t i = 0; i < notCovered.size(); i++) {
        notCoveredBalls[i] = aux[i].first;
    }
}

double min(double x1, double x2) {
    double conta = x1 - x2;
    if (conta <= 0) return x1;
    else return x2;
}

double max(double x1, double x2) {
    double conta = x1 - x2;
    if (conta <= 0) return x2;
    else return x1;
}

vector<int> BranchNBound::insert(vector<int> aux, int i, int k) {
    vector<int>::iterator it;
    it = aux.begin();
    for (int j = 0; j < i + 1; j++) {
        it++;
    }

    aux.insert(it, k);

    return aux;

}

void BranchNBound::computeLowerBounds(list<node *> *open_nodes, node *current_node, double *best_lb_aux) {
    list<node *>::iterator itOpen_aux;
    list<node *>::iterator aux;
    current_node->s_lb = 0;
    double min = DBL_MAX;

    for (itOpen_aux = (*open_nodes).begin(); itOpen_aux != (*open_nodes).end(); itOpen_aux++) {
        if ((*itOpen_aux)->lb < min) {
            min = (*itOpen_aux)->lb;
            aux = itOpen_aux;
        }
    }
    (*aux)->s_lb = 1;
    *best_lb_aux = min;
}

void BranchNBound::printLog(node *child_aux, list<node *> open_aux,
                            unsigned long int count_socp, double b_ub, double b_lb,
                            int notCovSize, int *printHeader2) {
    cout.unsetf(ios_base::floatfield);

    if (*printHeader2 == 0) {
        *printHeader2 = 1;

        cout << "F. value\t"
             << "BestUB\t"
             << "        BestLB\t"
             << "        GAP%(I)\t"
             << "        Level\t"
             << "#Uncov\t"
             << "#Solved\t"
             << " open\t"
             << "        Tree (%)\t" << endl;
    }
    cout << setfill('0') << setw(3) << child_aux->lb << "\t";
    if (b_ub > pow(10, 100)) {
        cout << setfill('\t') << setw(3) << "- " << "\t";
    } else {
        cout << setfill('0') << setw(3) << b_ub << "\t";
    }
    if (b_lb > pow(10, 100)) {
        cout << setfill('\t') << setw(3) << "- " << "\t";
    } else {
        cout << setfill('0') << setw(3) << b_lb << "\t";
    }
    double gap = ((b_ub - b_lb) / b_lb) * 100;
    if (gap > pow(10, 100) || gap == 0) {
        cout << setfill('\t') << setw(3) << "- " << "\t";
    } else {
        cout << setfill('0') << setw(3) << ((b_ub - b_lb) / b_lb) * 100 << "\t";
    }
    cout << setfill('0') << setw(3) << child_aux->pts.size() + 1 - 3 << "\t"
         << setfill('0') << setw(3) << notCovSize << "\t"
         << setfill('0') << setw(7) << count_socp << "\t "
         << setfill('0') << setw(7) << open_aux.size() << "\t"
         << setfill('0') << setw(3) << 0 << "%" << endl;
}

void BranchNBound::computeSizeTree(int sizeInst, mpz_t sizeOfTree, vector<mpz_class> &levels) {
    mpz_init(sizeOfTree);

    int heightOfTree = sizeInst - 2;

    for (int i = 0; i < heightOfTree; i++) {
        mpz_t factorial;
        mpz_init(factorial);
        unsigned long int n = i + 2;
        mpz_fac_ui(factorial, n);
        levels[i] = mpz_class(factorial) / 2;
        std::cout << "LEVELS " << i << " " << levels[i].get_ui() << std::endl;
        mpz_add(sizeOfTree, sizeOfTree, factorial);
        mpz_clear(factorial);
    }
    unsigned long int div = 2;
    mpz_div_ui(sizeOfTree, sizeOfTree, div);

    mpf_class print;
    print = mpz_class(sizeOfTree);

    //cout << scientific;
    //cout << "Tamanho da árvore: " << print << endl;
}

void BranchNBound::makeBranching([[maybe_unused]] branching *branchingS) {

}

bool BranchNBound::crossRoads([[maybe_unused]] vector<int> &sequence, [[maybe_unused]] vector<vector<double>> &solution) {
    return false;
}

void BranchNBound::setBranchingRuleList2() {
    vector<pair<int, double>> dist;
    dist.resize(sizeOfInstance);

    for (int i = 0; i < sizeOfInstance; i++) {
        (dist[i]).first = i;
        (dist[i]).second = sqrt(pow(objectOfData->getCoordx(0) - objectOfData->getCoordx(i), 2) +
                                pow(objectOfData->getCoordy(0) - objectOfData->getCoordy(i), 2));

    }

    sort(dist.begin(), dist.end(), sortVectorOfPairs);

    for (std::size_t i = 0; i < dist.size(); i++) {
        branchingRule2.push_back((dist[i]).first);
    }
}

int BranchNBound::strongBranching(list<int> nCovered, vector<int> sbSequence) {
    vector<pair<int, double>> sb_aux;
    pair<int, double> sbPair;
    list<int>::iterator sbIt;
    sbIt = nCovered.begin();
    int sbRange = 4;
    double sum = 0;

    for (int i = 0; i < sbRange; i++) {
        int k = (*sbIt);
        sbIt++;
        for (unsigned int j = 0; j < sbSequence.size(); j++) {
            node *sbChild = new node;
            sbChild->pts.push_back(0);
            sbChild->pts = insert(sbSequence, j, k);
            auto sbSOCP = new SolveSocpCplex(objectOfData, sbChild->pts);
            sbSOCP->solveSOCP(sbChild->pts);
            sbChild->lb = sbSOCP->getF_value();
            sum += sbChild->lb;
            delete sbChild;
        }
        sbPair = make_pair(k, sum);
        sb_aux.push_back(sbPair);
        sum = 0;
    }

    sort(sb_aux.begin(), sb_aux.end(), sortVectorOfPairs);
    cout << "teste: ";
    for (unsigned int i = 0; i < sb_aux.size(); i++) {
        cout << sb_aux[i].first << " ";
    }
    cout << endl;
    return sb_aux[0].first;
}



