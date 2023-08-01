add_executable(lower_bounds lower_bounds.cpp)
target_link_libraries(lower_bounds ${MOWING_LIBRARIES})
set_target_properties(lower_bounds PROPERTIES LINKER_LANGUAGE CXX)

add_executable(arkin_approximation approximation.cpp)
target_link_libraries(arkin_approximation ${APPROXIMATION_LIBRARIES})
set_target_properties(arkin_approximation PROPERTIES LINKER_LANGUAGE CXX)


add_executable(upper_bounds solve_from_lb_solution.cpp)
target_link_libraries(upper_bounds ${MOWING_LIBRARIES})
set_target_properties(upper_bounds PROPERTIES LINKER_LANGUAGE CXX)