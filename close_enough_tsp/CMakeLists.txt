add_library(close_enough_tsp
        src/BranchNBound.cpp
        src/Data.cpp
        src/CETSPSolver.cpp
        src/PrintFunctions.cpp
        src/SolveSocpCplex.cpp
        src/ShortestPathSolver.cpp
        src/util.cpp)
set_target_properties(close_enough_tsp PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(close_enough_tsp PUBLIC IL_STD)

util_setup_target(close_enough_tsp
        LIBRARIES ${CGAL_LIBRARY} ${GMP_LIBRARY} ${MPFR_LIBRARY}
        algutil::cplex
        algutil::ilconcert
        INCLUDE_DIRS ${INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CPLEX_INCLUDE_DIRS}
        ${CGAL_INCLUDE_DIR}
        ${GMP_INCLUDE_DIR}
        ${MPFR_INCLUDE_DIR}
        ${CMAKE_BASE_DIR}/utils/include)
set_target_properties(close_enough_tsp PROPERTIES LINKER_LANGUAGE CXX)