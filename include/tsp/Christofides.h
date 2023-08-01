#ifndef CETSP_CHRISTOFIDES_H
#define CETSP_CHRISTOFIDES_H

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
#include <cstdio>
#include <vector>
#include <limits>
#include "utils/cgal.h"

namespace tsp {
    class Christofides {
    private:

        typedef Kernel::FT NT;
        typedef Kernel::Point_2 Point;

        // List of odd nodes
        std::vector<std::size_t> odds;

        //Smaller cost matrix to find minimum matching on odd nodes
        NT **cost;

        void findOdds();

    public:
        std::size_t n;
        std::vector<std::pair<std::size_t, NT>> path_vals;
        NT pathLength;
        std::vector<std::size_t> circuit;
        std::vector<Point> cities;
        NT **graph;

        std::vector<std::size_t> *adjlist;

        Christofides(std::vector<Point> &points);

        ~Christofides();

        void perfectMatching();

        void euler_tour(std::size_t start, std::vector<std::size_t> &path) const;

        void make_hamiltonian(std::vector<std::size_t> &path, NT &pathCost);

        void findMST();

        std::size_t getMinIndex(NT key[], const bool mst[]);

        std::vector<std::size_t> solve();

        void fillMatrix();

        NT findBestPath(std::size_t start);
    };
}
#endif //CETSP_CHRISTOFIDES_H
