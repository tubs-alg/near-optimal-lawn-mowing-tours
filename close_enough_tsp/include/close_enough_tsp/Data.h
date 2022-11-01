#ifndef DATA_H
#define DATA_H

#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <numeric>
#include <utility>
#include <algorithm>    // std::sort
#include <cmath>
#include <boost/range/combine.hpp>

#include "constants.h"
#include "utils/utils.hpp"


class Data {

public:

    Data(std::vector<Point> &points,
               std::vector<double> &radii,
               std::vector<double> &demands,
               double overlapFactor, int instance_type = INSTANCE_TYPE_2D, bool addDepot = false);
    ~Data();

    //GET Functions
    int getSizeInst();

    double getCoordx(int);

    double getCoordy(int);

    double getCoordz(int);

    double getRadius(int);

    double getDemand(int);

    int getBranchingRuleList(int);

    int getDepotFarthest(int);

    double getOverlapRatio();

private:

    std::size_t sizeInst;
    double overlapRatio;
    double overlap_for_this_inst;

    std::vector<Point_3> points;

    std::vector<double> radii;
    std::vector<double> demands;

    std::vector<std::vector<int>> intersecMatrix;
    std::vector<int> countIntersecs;

    void setIntersecMatrix();

    std::list<int> branchingRuleList;
    std::list<int>::iterator iterBranchingRuleList;

    void setBranchingRuleList();

    void setDistanceMatrix();

    void setDepotFarthest();

    std::vector<std::vector<double>> distanceMatrix;
    std::vector<std::pair<int, double>> farthest;

};

struct pStruct {
    int index;
    int nOfIntersec;
    float radius;
    std::vector<int> intersections;
    int size;
};

struct coordinates {
    double x;
    double y;
    double z;
};


#endif
