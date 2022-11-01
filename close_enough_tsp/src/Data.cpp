#include "close_enough_tsp/Data.h"

using namespace std;

//constructor initializes sizeGraphMatrix, numberEdges, pNumber, pathMatrix and graphMatrix
Data::Data(std::vector<Point> &points,
           std::vector<double> &radii,
           std::vector<double> &demands,
           double overlapFactor,  [[maybe_unused]] int instance_type, bool addDepot) {

    this->overlapRatio = overlapFactor;

    if (addDepot) {
        this->points.emplace_back(0, 0, 0);
        this->radii.emplace_back(0);
        this->demands.emplace_back(0);

        this->sizeInst = points.size() + 1;
    } else {
        this->sizeInst = points.size();
    }



    double max_coordinate = 0;

    for (const auto tup : boost::combine(points, radii, demands)) {    // <---
        Point p;
        double r, d;
        boost::tie(p, r, d) = tup;

        this->points.emplace_back(p.x(), p.y(), 0);
        this->radii.emplace_back(r * overlapRatio);
        this->demands.emplace_back(d);

        max_coordinate = std::max(max_coordinate, CGAL::to_double(p.x()));
        max_coordinate = std::max(max_coordinate, CGAL::to_double(p.y()));
        max_coordinate = std::max(max_coordinate, CGAL::to_double(0));
    }

    setDistanceMatrix();
    setDepotFarthest();

    double sum_radii = std::accumulate(this->radii.begin(), this->radii.end(), 0.0);
    double avg_radius = sum_radii / ((double) sizeInst - 1);

    this->overlap_for_this_inst = avg_radius / max_coordinate;
}

// destrutor
Data::~Data() = default;

double Data::getOverlapRatio() {
    return overlap_for_this_inst;
}

void Data::setIntersecMatrix() {
    vector<int> temporary;

    long double test = 0;

    for (std::size_t i = 0; i < sizeInst; i++) {
        temporary.push_back(0);
    }

    for (std::size_t i = 0; i < sizeInst; i++) {
        intersecMatrix.push_back(temporary);
    }

    for (std::size_t i = 0; i < sizeInst; i++) {
        for (std::size_t j = i + 1; j < sizeInst; j++) {
            test = CGAL::to_double(CGAL::squared_distance(this->points[i], this->points[j])) -
                   (this->radii[i] + this->radii[j]);

            if (test <= 0) {
                intersecMatrix[i][j] = 1;
                intersecMatrix[j][i] = 1;
            } else {
                intersecMatrix[i][j] = 0;
                intersecMatrix[j][i] = 0;
            }
        }
    }

    for (std::size_t i = 0; i < sizeInst; i++) {
        for (std::size_t j = 0; j < sizeInst; j++) {
            countIntersecs.push_back(0);
            countIntersecs[i] += intersecMatrix[i][j];
        }
    }
}

bool compare(pStruct p1, pStruct p2) {
    for (int i = 0; i < p2.size && i != p2.index && p1.size != 0 && p2.size != 0; i++) {
        if (p2.intersections[i] == p1.index) {
            return true;
        }
    }

    if (p1.nOfIntersec > p2.nOfIntersec) {
        return true;
    }
    if (p1.nOfIntersec < p2.nOfIntersec) {
        return false;
    }

    if (p1.nOfIntersec == p2.nOfIntersec) {
        if (p1.radius > p1.radius) {
            return true;
        }
        if (p1.radius <= p1.radius) {
            return false;
        }
    }

    return false;
}

void Data::setBranchingRuleList() {
    pStruct p;
    list<pStruct> priorityList;
    list<pStruct>::iterator itPList;
    list<int>::iterator itBList;

    for (std::size_t i = 0; i < sizeInst; i++) {
        p.index = i;
        p.nOfIntersec = countIntersecs[i];
        p.radius = radii[i];
        p.intersections.clear();
        for (std::size_t j = 0; j < sizeInst; j++) {
            if (intersecMatrix[i][j] == 1) {
                p.intersections.push_back(j);
            }
        }
        p.size = p.intersections.size();

        priorityList.push_back(p);

    }

    itPList = priorityList.begin();
    priorityList.sort(compare);
    priorityList.splice(priorityList.begin(), priorityList, itPList);

    for (itPList = priorityList.begin(); itPList != priorityList.end(); itPList++) {
        branchingRuleList.push_back((*itPList).index);
    }
}

int Data::getBranchingRuleList(int i) {
    iterBranchingRuleList = branchingRuleList.begin();

    for (int j = 0; j < i; j++) {
        iterBranchingRuleList++;
    }

    return (*(iterBranchingRuleList));
}


void Data::setDistanceMatrix() {
    distanceMatrix.resize(sizeInst);

    for (std::size_t i = 0; i < sizeInst; i++) {
        distanceMatrix[i].resize(sizeInst);
    }

    for (std::size_t i = 0; i < sizeInst; i++) {
        for (std::size_t j = 0; j < sizeInst; j++) {
            distanceMatrix[i][j] = CGAL::to_double(CGAL::squared_distance(this->points[i], this->points[j]));
        }
    }
}

bool simpleSortVector(pair<int, double> i, pair<int, double> j) { return (i.second > j.second); }

void Data::setDepotFarthest() {
    pair<int, double> temp;

    for (std::size_t i = 0; i < sizeInst; i++) {
        temp = make_pair(i, distanceMatrix[0][i]);
        farthest.push_back(temp);
    }

    sort(farthest.begin(), farthest.end(), simpleSortVector);
}

int Data::getDepotFarthest(int i) {
    return farthest[i].first;
}

int Data::getSizeInst() {
    return sizeInst;
}

double Data::getCoordx(int i) {
    return CGAL::to_double(this->points[i].x());
}

double Data::getCoordy(int i) {
    return CGAL::to_double(this->points[i].y());
}

double Data::getCoordz(int i) {
    return CGAL::to_double(this->points[i].z());
}

double Data::getRadius(int i) {
    return this->radii[i];
}

double Data::getDemand(int i) {
    return this->demands[i];
}























