add_library(tsp
        src/TSPSolver.cpp
        src/Christofides.cpp)
set_target_properties(tsp PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(tsp
         ${gurobi_LIBRARIES} ${cgal_LIBRARIES} ${gmp_LIBRARIES} ${mpfr_LIBRARIES})
target_include_directories(
        tsp PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_BASE_DIR}/include
        ${gurobi_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${gmp_INCLUDE_DIRS}
        ${cgal_INCLUDE_DIRS})
set_target_properties(tsp PROPERTIES LINKER_LANGUAGE CXX)

set(TSP_INCLUDE_DIR ${CMAKE_BASE_DIR}/tsp/include)