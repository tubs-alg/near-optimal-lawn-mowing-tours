#ifndef structs_H
#define structs_H

#include <gmp.h>
#include <gmpxx.h>

#include "Data.h"
#include "BranchNBound.h"

struct node {
   vector< int > pts;
   vector< vector < double > > solXYZ;
   list< int > notCovered;
   double lb;
   int s_lb;
   int feasible;
};

struct sbAuxStruct {
   int index;
   double sum;
   vector< node* > candidates;

};

struct branching {
   Data * dataptr;
   node * current;
   mpz_class quantity;
   mpf_class temp;
   mpz_t sizeOftree;
   vector< mpz_class > * levels;
   vector< int > * solucao;
   vector< vector< double > > * solutionXYZ;
   list< node* > * open;
   int * k;
   int * eliminatedNodesCounter;
   long unsigned int * count_SOCP_solved;
   double * ub;
   double * best_ub;
   double * best_lb;
   double * best;
};

struct myCompareStruct
{
   vector< int > vectorOfNotCov;
   vector< pair< int, double > > sortedNotCov;

   myCompareStruct( vector< int > a, vector< pair< int, double > > s )
      : vectorOfNotCov( a ), sortedNotCov( s ){
      }

   bool operator() ( pair< int, double > i, pair< int, double > j ){ 
      return i.second > j.second;
   }
};

#endif
