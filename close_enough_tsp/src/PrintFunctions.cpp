#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <cstdlib>
#include <stdio.h>
#include <cfloat>
#include <gmp.h>
#include <gmpxx.h>
#include <math.h>

#include "close_enough_tsp/PrintFunctions.h"
#include "close_enough_tsp/util.h"
#include "close_enough_tsp/Data.h"

void printDataToFile(Data *dataptr, char *option, double overlap) {
    ofstream outFile("Resultados/resultados_BnB_CETSP_OPTF.txt", ios::app);

    if (!outFile) {
        cout << "arquivo nao pode ser criado\n";
        exit(1);
    }

    if (!outFile) {
        cout << "arquivo nao pode ser criado\n";
        exit(1);
    }

    outFile << fixed << setiosflags(ios::showpoint) << setprecision(2);

    outFile << option << " "
            << overlap << " "
            << dataptr->getOverlapRatio() << " ";
    outFile << endl;
    outFile.close();
}

void printDataToFile(Data *dataptr, double overlap, int sizeInst, double bestKnown, double best,
                     double best_lb, double gap_root, int count_SOCP_solved, int itCount, double computationTime,
                     double sbComputationTime, double totalSocpCompTime) {
    ofstream outFile("Resultados/resultados_BnB_CETSP_OPTF.txt", ios::app);

    if (!outFile) {
        cout << "arquivo nao pode ser criado\n";
        exit(1);
    }

    outFile << fixed << setiosflags(ios::showpoint) << setprecision(15);

    outFile << "OPTF" << " "
            << overlap << " "
            << dataptr->getOverlapRatio() << " "
            << sizeInst << " "
            << bestKnown << " "
            << best << " ";

    if (best_lb >= 999999999999999) outFile << " - ";
    else outFile << best_lb << " ";

    outFile
            << gap_root << " "
            << count_SOCP_solved << " "
            << itCount << " "
            << computationTime << " "
            << sbComputationTime << " "
            << totalSocpCompTime << " ";
    outFile << endl;
    outFile.close();
}

void
printDataToFile(Data *dataptr, double overlap, int sizeInst, double bestKnown, double ub, double best_lb,
                double gap_real, double gap_lb_bnb, double gap_root, int count_SOCP_solved, int itCount,
                double computationTime, double sbComputationTime, double totalSocpCompTime) {
    ofstream outFile("Resultados/resultados_BnB_CETSP_noOPTF.txt", ios::app);

    if (!outFile) {
        cout << "arquivo nao pode ser criado\n";
        exit(1);
    }

    outFile << fixed << setiosflags(ios::showpoint) << setprecision(15);

    outFile << "noOPTF" << " "
            << overlap << " "
            << dataptr->getOverlapRatio() << " "
            << sizeInst << " "
            << bestKnown << " "
            << ub << " "
            << best_lb << " "
            << gap_real << " "
            << gap_lb_bnb << " "
            << gap_root << " "
            << count_SOCP_solved << " "
            << itCount << " "
            << computationTime << " "
            << sbComputationTime << " "
            << totalSocpCompTime << " ";
    outFile << endl;
    outFile.close();
}

