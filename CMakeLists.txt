set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

cmake_minimum_required(VERSION 3.16)

project(lawn_mowing)

set(CMAKE_POLICY_DEFAULT_CMP0115 OLD) # Allows addition of directories instead of individual files inside cmake.
set(CMAKE_POLICY_DEFAULT_CMP0076 OLD)
#set(CMAKE_OSX_ARCHITECTURES x86_64)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # not necessary, but encouraged
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-deprecated-copy-with-user-provided-copy -std=c++17 -frounding-math -fvisibility=hidden")
set(target_link_options "--as-needed")

set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake" NO_POLICY_SCOPE)
set(_CPLEX_CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(gurobi REQUIRED)
find_package(gmp REQUIRED)
find_package(mpfr REQUIRED)
find_package(CGAL REQUIRED)
find_package(jsoncpp REQUIRED)

add_subdirectory(close_enough_tsp)
add_subdirectory(tsp)
add_subdirectory(approximation)
add_subdirectory(mowing)

message(STATUS "CGAL libs ${cgal_LIBRARIES}")
message(STATUS "Gurobi libs ${gurobi_LIBRARIES}")
message(STATUS "GMP libs ${gmp_LIBRARIES}")
message(STATUS "MPFR libs ${mpfr_LIBRARIES}")
message(STATUS "JSONCPP libs ${jsoncpp_LIBRARIES}")
message(STATUS "doctest libs ${doctest_LIBRARIES}")

set(MOWING_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/mowing/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/include
        ${jsoncpp_INCLUDE_DIRS})

set(APPROXIMATION_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/approximation/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/include
        ${jsoncpp_INCLUDE_DIRS})

set(MOWING_LIBRARIES mowing ${jsoncpp_LIBRARIES})
set(APPROXIMATION_LIBRARIES approximation ${jsoncpp_LIBRARIES})

add_executable(lower_bounds lower_bounds.cpp)
target_link_libraries(lower_bounds ${MOWING_LIBRARIES})
target_include_directories(lower_bounds PUBLIC ${MOWING_INCLUDE_DIRS})
set_target_properties(lower_bounds PROPERTIES LINKER_LANGUAGE CXX)

add_executable(arkin_approximation approximation.cpp)
util_setup_target(arkin_approximation
        LIBRARIES ${APPROXIMATION_LIBRARIES}
        INCLUDE_DIRS ${APPROXIMATION_INCLUDE_DIRS})
set_target_properties(arkin_approximation PROPERTIES LINKER_LANGUAGE CXX)


add_executable(upper_bounds solve_from_lb_solution.cpp)
util_setup_target(upper_bounds
        LIBRARIES ${MOWING_LIBRARIES}
        INCLUDE_DIRS ${MOWING_INCLUDE_DIRS})
set_target_properties(upper_bounds PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(tests)