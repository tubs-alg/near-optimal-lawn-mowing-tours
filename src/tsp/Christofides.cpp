#include "tsp/Christofides.h"


namespace tsp {
    std::vector<std::size_t> Christofides::solve() {
        // Fill N x N matrix with distances between nodes
        std::cout << "Fillmatrix started" << std::endl;
        this->fillMatrix();
        std::cout << "Filled Matrix" << std::endl;

        // Find a MST T in graph G
        this->findMST();
        std::cout << "MST created" << std::endl;

        // Find a minimum weighted matching M for odd vertices in T
        this->perfectMatching();
        std::cout << "Matching completed" << std::endl;

        // Loop through each index and find shortest path
        NT best = INT_MAX;
        std::size_t bestIndex;
        for (std::size_t t = 0; t < this->n; t++) {
            NT result = this->findBestPath(t);

            this->path_vals[t].first = t; // set start
            this->path_vals[t].second = result; // set end

            if (this->path_vals[t].second < best) {
                bestIndex = this->path_vals[t].first;
                best = this->path_vals[t].second;
            }
        }

        //Create path for best tour
        this->euler_tour(bestIndex, this->circuit);
        this->make_hamiltonian(this->circuit, this->pathLength);

        /*
        tsp.euler_tour(0, tsp.circuit);
        std::cout << "euler completed" << std::endl;
        tsp.make_hamiltonian(tsp.circuit, tsp.pathLength);
        std::cout << "ham completed" << std::endl;
        */

        // Store best path
        //tsp.create_tour(bestIndex);
        /*
        std::cout << "Final length: " << this->pathLength << std::endl;

        for (auto it = circuit.begin(); it != circuit.end() - 1; ++it) {
            std::cout << *it << " to " << *(it + 1) << " ";
            std::cout << graph[*it][*(it + 1)] << std::endl;
        }
        std::cout << *(circuit.end() - 1) << " to " << circuit.front();*/

        return circuit;
    }

//Constructor
    Christofides::Christofides(std::vector<Point> &points) {

        this->cities = points;

        //Initialize member variables
        this->n = points.size();
        this->graph = new NT *[n];
        for (std::size_t i = 0; i < n; i++) {
            this->graph[i] = new NT[n];
            for (std::size_t j = 0; j < n; j++) {
                this->graph[i][j] = 0;
            }
        }

        this->cost = new NT *[n];
        for (std::size_t i = 0; i < n; i++) {
            this->cost[i] = new NT[n];
        }

        this->path_vals = std::vector<std::pair<std::size_t, NT>>();

        for (std::size_t t = 0; t < this->n; t++) {
            this->path_vals.emplace_back(INT_MAX, INT_MAX);
        }
        this->adjlist = new std::vector<std::size_t>[n];
    }

//Destructor
    Christofides::~Christofides() {
        for (std::size_t i = 0; i < n; i++) {
            delete[] graph[i];
            delete[] cost[i];
        }
        delete[] graph;
        delete[] cost;
        delete[] adjlist;
    }

    void Christofides::fillMatrix() {
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++) {
                graph[i][j] = graph[j][i] = CGAL::squared_distance(cities[i], cities[j]);
            }
        }
    }


/******************************************************************************
  This function uses Prim's algorithm to determine a minimum spanning tree on
    the graph
******************************************************************************/

    void Christofides::findMST() {

        auto key = new NT[n];
        bool *included = new bool[n];
        auto parent = new std::size_t[n];

        for (std::size_t i = 0; i < n; i++) {

            // set each key to infinity
            key[i] = std::numeric_limits<int>::max();
            parent[i] = this->n;
            // node node yet included in MST
            included[i] = false;

        }

        // root of MST has distance of 0 and no parent
        key[0] = 0;
        parent[0] = -1;

        for (std::size_t i = 0; i < n - 1; i++) {

            // find closes vertex not already in tree
            auto k = getMinIndex(key, included);

            // set included to true for this vertex
            included[k] = true;

            // examine each unexamined vertex adjacent to most recently added
            for (std::size_t j = 0; j < n; j++) {

                // node exists, is unexamined, and graph[k][j] less than previous
                // key for u
                if (graph[k][j] > 0 && !included[j] && graph[k][j] < key[j]) {

                    // update parent
                    parent[j] = k;

                    // update key
                    key[j] = graph[k][j];

                }
            }

        }

        // construct a tree by forming adjacency matrices
        for (std::size_t i = 0; i < n; i++) {
            auto j = parent[i];

            if (j < this->n) {
                adjlist[i].push_back(j);
                adjlist[j].push_back(i);
            }

        }

    }


/******************************************************************************
  find the index of the closest unexamined node
******************************************************************************/

    std::size_t Christofides::getMinIndex(NT key[], const bool mst[]) {

        // initialize min and min_index
        NT min = std::numeric_limits<int>::max();
        std::size_t min_index = this->n;

        // iterate through each vertex
        for (std::size_t i = 0; i < n; i++) {

            // if vertex hasn't been visited and has a smaller key than min
            if (!mst[i] && key[i] < min) {

                // reassign min and min_index to the values from this node
                min = key[i];
                min_index = i;

            }

        }

        return min_index;

    }


/******************************************************************************
  find all vertices of odd degree in the MST. Store them in an subgraph O
******************************************************************************/

    void Christofides::findOdds() {
        for (std::size_t i = 0; i < n; i++) {
            // if degree of vertex i is odd
            if ((adjlist[i].size() % 2) != 0) {
                // push vertex to odds, which is a representation of subgraph O
                odds.push_back(i);
            }
        }
    }


    void Christofides::perfectMatching() {
        /************************************************************************************
         find a perfect matching M in the subgraph O using greedy algorithm but not minimum
        *************************************************************************************/
        NT length; //int d;
        std::size_t closest;
        std::vector<std::size_t>::iterator tmp, first;

        // Find nodes with odd degrees in T to get subgraph O
        findOdds();

        // for each odd node
        while (!odds.empty()) {
            first = odds.begin();
            auto it = odds.begin() + 1;
            auto end = odds.end();
            length = std::numeric_limits<int>::max();
            for (; it != end; ++it) {
                // if this node is closer than the current closest, update closest and length
                if (graph[*first][*it] < length) {
                    length = graph[*first][*it];
                    closest = *it;
                    tmp = it;
                }
            } // two nodes are matched, end of list reached
            adjlist[*first].push_back(closest);
            adjlist[closest].push_back(*first);
            odds.erase(tmp);
            odds.erase(first);
        }
    }


//find an euler circuit
    void Christofides::euler_tour(std::size_t start, std::vector<std::size_t> &path) const {
        //Create copy of adj. list
        auto tempList = new std::vector<std::size_t>[n];
        for (std::size_t i = 0; i < n; i++) {
            tempList[i].resize(adjlist[i].size());
            tempList[i] = adjlist[i];
        }

        std::stack<int> stack;
        std::size_t pos = start;
        path.push_back(start);
        while (!stack.empty() || !tempList[pos].empty()) {
            //Current node has no neighbors
            if (tempList[pos].empty()) {
                //add to circuit
                path.push_back(pos);
                //remove last vertex from stack and set it to current
                pos = stack.top();
                stack.pop();
            }
                //If current node has neighbors
            else {
                //Add vertex to stack
                stack.push(pos);
                //Take a neighbor
                int neighbor = tempList[pos].back();
                //Remove edge between neighbor and current vertex
                tempList[pos].pop_back();
                for (std::size_t i = 0; i < tempList[neighbor].size(); i++) {
                    if (tempList[neighbor][i] == pos) {
                        tempList[neighbor].erase(tempList[neighbor].begin() + i);
                    }
                }
                //Set neighbor as current vertex
                pos = neighbor;
            }
        }
        path.push_back(pos);
    }


//Make euler tour Hamiltonian
    void Christofides::make_hamiltonian(std::vector<std::size_t> &path, NT &pathCost) {

        //remove visited nodes from Euler tour
        bool *visited = new bool[n];
        for (std::size_t i = 0; i < n; i++) {
            visited[i] = false;
        }

        pathCost = 0;

        int root = path.front();
        auto cur = path.begin();
        auto iter = path.begin() + 1;
        visited[root] = true;

        //iterate through circuit

        while (iter != path.end()) {
            if (!visited[*iter]) {
                pathCost += graph[*cur][*iter];
                cur = iter;
                visited[*cur] = true;
                iter = cur + 1;
            } else {
                iter = path.erase(iter);
            }
        }
        //Add distance to root
        if (iter != path.end()) {
            pathCost += graph[*cur][*iter];
        }
    }

    Christofides::NT Christofides::findBestPath(std::size_t start) {
        std::vector<std::size_t> path;
        euler_tour(start, path);
        NT length;

        make_hamiltonian(path, length);
        return length;
    }
}