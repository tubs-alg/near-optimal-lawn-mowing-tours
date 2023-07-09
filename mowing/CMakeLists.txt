add_library(mowing
        src/utils/mowing.cpp
        src/utils/cetsp_tours.cpp
        src/ExactOffsetCalculator.cpp
        src/LowerBoundSolver.cpp
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
target_link_libraries(mowing PUBLIC
        cetsp tsp
        gurobi::gurobi)
target_include_directories(mowing PUBLIC
        ${INCLUDE_DIRS}
        ${TSP_INCLUDE_DIR}
        ${CMAKE_BASE_DIR}/close_enough_tsp/include
        ${CMAKE_BASE_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_CURRENT_LIST_DIR}/include)
set_target_properties(mowing PROPERTIES LINKER_LANGUAGE CXX)