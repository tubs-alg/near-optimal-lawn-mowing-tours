# Lawn Mowing Solver
This repository contains the code for a solver to the lawn mowing problem.
For comparison we have also implemented a well-known approximation algorithm from Arkin, Fekete and Mitchell [1].
The code will be updated with comments in the upcoming weeks.


Our approach makes use of optimal solutions to the Close-Enough TSP.
We modified an existing solver from Couthino et al. [2] that is available on [Github](https://github.com/waltonpcoutinho/BnB_CETSP) so that we can use it in our implementation.
For solving the TSP, our exact solver uses Christofides approximation as a start solution that was adapted from [this](https://github.com/sth144/christofides-algorithm-cpp) repository.

### Setup

Note that this code needs additional libraries to work: The commercial CPLEX and Gurobi library. All other dependencies can be installed
using the Conan package manager
```
conan install . -of cmake-build-debug --build=missing -s build_type=Debug
```

The executable targets are `arkin_approximation`, `lower_bounds` and `upper_bounds`.

* `arkin_approximation`: for executing the approximation
algorithm of Arkin, Fekete and Mitchell [1].
* `lower_bounds`: for computing lower bounds with various witness placement strategies.
* `upper_bounds`: for computing upper bounds from a lower bound solution, i.e. make a given tour feasible.

Additional test cases ensure that the code is running properly.

### References
[1] Arkin, Esther M., Sándor P. Fekete, and Joseph SB Mitchell. "[Approximation algorithms for lawn mowing and milling.](https://www.sciencedirect.com/science/article/pii/S0925772100000158)" Computational Geometry 17.1-2 (2000): 25-50.

[2] Coutinho, W. P., Nascimento, R. Q. D., Pessoa, A. A., & Subramanian, A. (2016). [A branch-and-bound algorithm for the close-enough traveling salesman problem.](https://pubsonline.informs.org/doi/abs/10.1287/ijoc.2016.0711) INFORMS Journal on Computing, 28(4), 752-765.