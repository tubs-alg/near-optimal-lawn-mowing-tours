#ifndef BranchNBound_H
#define BranchNBound_H

#include <cmath>
#include <string>
#include <iostream>
#include<iomanip>
#include<fstream>
#include<vector>
#include<list>
#include<cstdlib>
#include<ctime>
#include<climits>
#include <cfloat>
#include<algorithm>
#include <stdio.h>

#include"SolveSocpCplex.h"
#include"Data.h"
#include"structs.h"
#include"util.h"

using namespace std;

class BranchNBound {

public:

    BranchNBound(Data *); //constructor
    ~BranchNBound();

    vector<int> selectRoot();

    vector<int> selectRoot2();

    vector<int> selectRoot3();

    bool check_feasibility_Q(vector<int>, vector<vector<double> >);

    vector<int> insert(vector<int>, int, int);

    void computeSizeTree(int sizeInst, mpz_t sizeOfTree, vector<mpz_class> &levels);

    void computeLowerBounds(list<node *> *, node *, double *);

    void printLog(node *, list<node *>, unsigned long int, double, double, int, int *);


    bool checkFeasibility(vector<vector<double> >, vector<int>); // not using
    void bnb_algorithm(); //not using
    int getNotCoveredBalls(int i); // not using
    bool crossRoads(vector<int> &, vector<vector<double> > &); // not using
    void setBranchingRuleList2(); // not using
    int strongBranching(list<int>, vector<int>); // not using
    void makeBranching(branching *); // not using

    vector<int> notCoveredBalls;
    list<int> branchingRule2;

private:
    Data *objectOfData;
    SolveSocpCplex *objectOfSolveCplex;

    int branching_rule;

    //	SET FUNCTIONS

    int sizeOfInstance;
    vector<int> root;

    void sortNotCovered(vector<int> notCovered, vector<vector<double> > solutionTemp);

    void sortNotCovered2(vector<int> notCovered, vector<vector<double> > solutionTemp);

};

struct point {
    double x;
    double y;
    double z;
};

#endif
