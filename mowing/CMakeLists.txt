add_library(mowing
        src/mowing.cpp
        src/utils/cetsp_tours.cpp
        src/ExactOffsetCalculator.cpp
        src/TSPSolver.cpp
        src/LowerBoundSolver.cpp
        src/Christofides.cpp
        src/FeketeApproximation.cpp
        src/FeasibleToursFromLowerBound.cpp
        src/MowingSolver.cpp
        src/MowingSolverWithUpperBound.cpp
        src/utils/cetsp_tours.cpp
        src/witnesses/placement/WitnessPlacementStrategy.cpp
        src/witnesses/placement/initial/ConvexHullPlacementStrategy.cpp
        src/witnesses/placement/initial/PolygonVerticesPlacementStrategy.cpp
        src/witnesses/placement/followup/StraightSkeletonPlacementStrategy.cpp
        src/witnesses/placement/followup/RandomPlacementStrategy.cpp
        src/witnesses/placement/followup/GridPlacementStrategy.cpp
        src/witnesses/sparsification/DispersionSparsification.cpp
        src/witnesses/sparsification/KMeansSparsification.cpp
        src/utils/conversion.cpp)
set_target_properties(mowing PROPERTIES LINKER_LANGUAGE CXX)
util_setup_target(mowing
        LIBRARIES close_enough_tsp ${GUROBI} ${_GUROBI_CPP_STATIC_LIB} ${_GUROBI_DLL}
        INCLUDE_DIRS ${INCLUDE_DIRS}
        ${CGAL_INCLUDE_DIR}
        ${_GUROBI_CPP_INCLUDE_DIR}
        ${CMAKE_BASE_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_CURRENT_LIST_DIR}/include)
set_target_properties(mowing PROPERTIES LINKER_LANGUAGE CXX)