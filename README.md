# Lawn Mowing Solver
This repository contains the code for a solver to the lawn mowing problem.
For comparison we have also implemented a well-known approximation algorithm from Arkin, Fekete and Mitchell [1].
The code will be updated with comments in the upcoming weeks.

Note that this code needs additional libraries to work: The commercial CPLEX and Gurobi library.

Our approach makes use of optimal solutions to the Close-Enough TSP.
We modified an existing solver from Couthino et al. [2] that is available on [Github](https://github.com/waltonpcoutinho/BnB_CETSP) so that we can use it in our implementation.
For solving the TSP, our exact solver uses Christofides approximation as a start solution that was adapted from [this](https://github.com/sth144/christofides-algorithm-cpp) repository.

### References
[1] Arkin, Esther M., Sándor P. Fekete, and Joseph SB Mitchell. "[Approximation algorithms for lawn mowing and milling.](https://www.sciencedirect.com/science/article/pii/S0925772100000158)" Computational Geometry 17.1-2 (2000): 25-50.

[2] Coutinho, W. P., Nascimento, R. Q. D., Pessoa, A. A., & Subramanian, A. (2016). [A branch-and-bound algorithm for the close-enough traveling salesman problem.](https://pubsonline.informs.org/doi/abs/10.1287/ijoc.2016.0711) INFORMS Journal on Computing, 28(4), 752-765.