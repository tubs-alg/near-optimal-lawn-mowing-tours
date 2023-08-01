add_library(cetsp
        ${INCLUDE_DIRECTORY}/cetsp/heuristics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/heuristics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
        ${INCLUDE_DIRECTORY}/cetsp/node.h
        ${INCLUDE_DIRECTORY}/cetsp/bnb.h
        ${INCLUDE_DIRECTORY}/cetsp/strategies/root_node_strategy.h
        ${INCLUDE_DIRECTORY}/cetsp/details/solution_pool.h
        ${INCLUDE_DIRECTORY}/cetsp/strategies/branching_strategy.h
        ${INCLUDE_DIRECTORY}/cetsp/strategies/search_strategy.h
        ${INCLUDE_DIRECTORY}/cetsp/callbacks.h
        ${CMAKE_CURRENT_SOURCE_DIR}/root_node_strategies/convex_hull_root.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/branching_strategy.cpp
        ${INCLUDE_DIRECTORY}/cetsp/details/triple_map.h
        ${INCLUDE_DIRECTORY}/cetsp/details/convex_hull_order.h
        ${CMAKE_CURRENT_SOURCE_DIR}/convex_hull_order.cpp
        ${INCLUDE_DIRECTORY}/cetsp/utils/timer.h
        ${INCLUDE_DIRECTORY}/cetsp/relaxed_solution.h
        ${INCLUDE_DIRECTORY}/cetsp/details/distance_cache.h
        ${CMAKE_CURRENT_SOURCE_DIR}/relaxed_solution.cpp
        ${INCLUDE_DIRECTORY}/cetsp/details/lazy_trajectory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/geometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/root_node_strategies/longest_edge_plus_farthest_circle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/branching_strategies/global_convex_hull.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/branching_strategies/layered_convex_hull_rule.cpp
        ${INCLUDE_DIRECTORY}/cetsp/strategies/rule.h
        ${INCLUDE_DIRECTORY}/cetsp/strategies/rules/global_convex_hull_rule.h
        ${INCLUDE_DIRECTORY}/cetsp/solver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/heuristics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/root_node_strategies/convex_hull_root.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/branching_strategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/convex_hull_order.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/relaxed_solution.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/root_node_strategies/longest_edge_plus_farthest_circle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/branching_strategies/global_convex_hull.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/branching_strategies/layered_convex_hull_rule.cpp)
target_sources(
  cetsp
  PUBLIC ${INCLUDE_DIRECTORY}/cetsp/common.h ${INCLUDE_DIRECTORY}/cetsp/details/cgal_kernel.h
        ${INCLUDE_DIRECTORY}/cetsp/soc.h ${INCLUDE_DIRECTORY}/cetsp/solver.h
  PRIVATE soc.cpp)
target_include_directories(cetsp PUBLIC ${INCLUDE_DIRECTORY})
set_target_properties(cetsp PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(cetsp PUBLIC ${gurobi_LIBRARIES}
        ${cgal_LIBRARIES} ${gmp_LIBRARIES} ${mpfr_LIBRARIES} ${Boost_LIBRARIES})
target_compile_definitions(cetsp PRIVATE DOCTEST_CONFIG_DISABLE)
