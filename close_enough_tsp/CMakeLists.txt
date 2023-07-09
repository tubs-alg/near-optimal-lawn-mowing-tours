add_library(
  cetsp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/heuristics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/heuristics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/node.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/bnb.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/strategies/root_node_strategy.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/details/solution_pool.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/strategies/branching_strategy.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/strategies/search_strategy.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/callbacks.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/root_node_strategies/convex_hull_root.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/branching_strategy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/details/triple_map.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/details/convex_hull_order.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/convex_hull_order.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/utils/timer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/relaxed_solution.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/details/distance_cache.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/relaxed_solution.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/details/lazy_trajectory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/root_node_strategies/longest_edge_plus_farthest_circle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/branching_strategies/global_convex_hull.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/branching_strategies/layered_convex_hull_rule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/strategies/rule.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/strategies/rules/global_convex_hull_rule.h include/cetsp/solver.h)
target_sources(
  cetsp
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/common.h ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/details/cgal_kernel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/soc.h ${CMAKE_CURRENT_SOURCE_DIR}/include/cetsp/solver.h
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/soc.cpp)
target_include_directories(cetsp PUBLIC include)
set_target_properties(cetsp PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(cetsp PUBLIC ${gurobi_LIBRARIES}
        ${cgal_LIBRARIES} ${gmp_LIBRARIES} ${mpfr_LIBRARIES} ${Boost_LIBRARIES})
target_compile_definitions(cetsp PRIVATE DOCTEST_CONFIG_DISABLE)
