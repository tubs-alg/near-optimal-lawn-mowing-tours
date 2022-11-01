add_library(tsp
        src/TSPSolver.cpp
        src/Christofides.cpp)
set_target_properties(tsp PROPERTIES LINKER_LANGUAGE CXX)
util_setup_target(tsp
        LIBRARIES ${GUROBI} ${_GUROBI_CPP_STATIC_LIB} ${_GUROBI_DLL} ${CGAL_LIBRARY} ${GMP_LIBRARY} ${MPFR_LIBRARY}
        INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_BASE_DIR}/include
        ${_GUROBI_CPP_INCLUDE_DIR}
        ${CGAL_INCLUDE_DIR})
        #${CGAL_INCLUDE_DIR}
        #
        #${CMAKE_BASE_DIR}/include
        #${CMAKE_BASE_DIR}/utils/include
        #${CMAKE_CURRENT_LIST_DIR}/include)
set_target_properties(tsp PROPERTIES LINKER_LANGUAGE CXX)

set(TSP_INCLUDE_DIR ${CMAKE_BASE_DIR}/tsp/include)