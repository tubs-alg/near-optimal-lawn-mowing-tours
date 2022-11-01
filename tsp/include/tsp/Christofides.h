#ifndef CETSP_CHRISTOFIDES_H
#define CETSP_CHRISTOFIDES_H

#include "utils/utils.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <stdio.h>
#include <vector>
#include <limits>

class Christofides
{
private:

    typedef Kernel::FT NT;
    typedef Kernel::Point_2 Point;

    // List of odd nodes
    std::vector<std::size_t>odds;

    //Smaller cost matrix to find minimum matching on odd nodes
    NT **cost;

    void findOdds();
public:
    // Number of cities
    std::size_t n;

    //path
    std::vector<std::pair<std::size_t, NT>> path_vals;

    //Shortest path length
    NT pathLength;

    //euler circuit
    std::vector<std::size_t> circuit;

    std::vector<Point> cities;

    // n x n, pairwise distances between cities
    NT **graph;

    std::vector<std::size_t>* adjlist;

    // Constructor
    Christofides(std::vector<Point> &points);

    // Destructor
    ~Christofides();

    //Find perfect matching
    void perfectMatching();

    //Find Euler tour
    void euler_tour(std::size_t start, std::vector<std::size_t> &path) const;

    //Find Hamiltonian path
    void make_hamiltonian(std::vector<std::size_t> &path, NT &pathCost);

    // Prim's algorithm
    void findMST();

    std::size_t getMinIndex(NT key[], const bool mst[]);

    std::vector<std::size_t> solve();

    void fillMatrix();

    NT findBestPath(std::size_t start);

};


#endif //CETSP_CHRISTOFIDES_H
