#ifndef SOLVESOCPCPLEX_H
#define SOLVESOCPCPLEX_H

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <algorithm>
#include <stdio.h>
#include <ilcplex/ilocplex.h>
#include <pthread.h>

#include "close_enough_tsp/Data.h"

using namespace std;

class SolveSocpCplex {

public:

    //construtor
    SolveSocpCplex(Data *, vector<int>); //constructor
    ~SolveSocpCplex();

    //functions
    void solveSOCP(vector<int>);

    void finishSOCP();

    double getF_value();

    double getSolutionX(int);

    double getSolutionY(int);

    double getSolutionZ(int);

    void printF_value();

    void printSolution(vector<int>);

    IloNum violation;

private:

    IloEnv env;
    IloModel model;
    IloCplex SOCP;
    IloNumArray xCoord;
    IloNumArray yCoord;
    IloNumArray zCoord;
    IloNumVarArray x;
    IloNumVarArray y;
    IloNumVarArray z;

    Data *objectData;

    int sizeProblem;
    double f_value;

    //functions for solving SOCP
    int setSizeProblem(vector<int>);

    void createModel(vector<int>);

    void setF_value();

};

#endif
