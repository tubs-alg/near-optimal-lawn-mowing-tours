add_library(mowing
        utils/mowing.cpp
        utils/cetsp_tours.cpp
        ExactOffsetCalculator.cpp
        LowerBoundSolver.cpp
        FeasibleToursFromLowerBound.cpp
        MowingSolver.cpp
        MowingSolverWithUpperBound.cpp
        utils/cetsp_tours.cpp
        witnesses/placement/WitnessPlacementStrategy.cpp
        witnesses/placement/initial/ConvexHullPlacementStrategy.cpp
        witnesses/placement/initial/PolygonVerticesPlacementStrategy.cpp
        witnesses/placement/followup/StraightSkeletonPlacementStrategy.cpp
        witnesses/placement/followup/RandomPlacementStrategy.cpp
        witnesses/placement/followup/GridPlacementStrategy.cpp
        witnesses/sparsification/DispersionSparsification.cpp
        witnesses/sparsification/KMeansSparsification.cpp
        utils/conversion.cpp)
target_link_libraries(mowing PUBLIC
        cetsp tsp
        ${gurobi_LIBRARIES})
target_include_directories(mowing PUBLIC ${INCLUDE_DIR})
set_target_properties(mowing PROPERTIES LINKER_LANGUAGE CXX)


# Create a new Python-module ~~~~~~~~~~~~~~~~~~~~~
pybind11_add_module(
        _mowing_bindings # target name with leading `_` as this will be a protected
        # module.
        _mowing_bindings.cpp) # the file with the API.
target_link_libraries(_mowing_bindings PRIVATE
        mowing
        ${pybind11_LIBRARIES}
        ${pybind11_json_LIBRARIES}
        cetsp
        tsp
        ${gurobi_LIBRARIES})
# enable compilation warnings
target_compile_options(_mowing_bindings PRIVATE "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>")
target_compile_definitions(_mowing_bindings PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)

install(TARGETS _mowing_bindings DESTINATION ${CMAKE_EVALUATION_DIR})