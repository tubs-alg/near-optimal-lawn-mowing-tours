add_executable(test_validate_solutions validate_solutions.cpp)
util_setup_target(test_validate_solutions
        LIBRARIES ${MOWING_LIBRARIES}
        INCLUDE_DIRS ${MOWING_INCLUDE_DIRS})
set_target_properties(test_validate_solutions PROPERTIES LINKER_LANGUAGE CXX)


add_executable(test_validate_upper_bounds validate_upper_bounds.cpp)
util_setup_target(test_validate_upper_bounds
        LIBRARIES ${MOWING_LIBRARIES}
        INCLUDE_DIRS ${MOWING_INCLUDE_DIRS})
set_target_properties(test_validate_upper_bounds PROPERTIES LINKER_LANGUAGE CXX)