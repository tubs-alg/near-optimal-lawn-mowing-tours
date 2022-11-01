add_library(approximation src/FeketeApproximation.cpp)
set_target_properties(approximation PROPERTIES LINKER_LANGUAGE CXX)
util_setup_target(approximation
        LIBRARIES tsp
        INCLUDE_DIRS
        ${INCLUDE_DIRS}
        ${TSP_INCLUDE_DIR}
        ${CGAL_INCLUDE_DIR}
        #${_GUROBI_CPP_INCLUDE_DIR}
        #${CMAKE_BASE_DIR}/include
        #${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_CURRENT_LIST_DIR}/include)
set_target_properties(approximation PROPERTIES LINKER_LANGUAGE CXX)