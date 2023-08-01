set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

cmake_minimum_required(VERSION 3.16)

project(lawn_mowing)

set(CMAKE_POLICY_DEFAULT_CMP0115 OLD) # Allows addition of directories instead of individual files inside cmake.
set(CMAKE_POLICY_DEFAULT_CMP0076 OLD)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # not necessary, but encouraged
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # The code needs to be compiled as PIC
# to build the shared lib for python.
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-deprecated-copy-with-user-provided-copy -frounding-math -fvisibility=hidden")
set(target_link_options "--as-needed")

set(CMAKE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EVALUATION_DIR ${CMAKE_BASE_DIR}/evaluation)

find_package(gurobi REQUIRED)
find_package(gmp REQUIRED)
find_package(mpfr REQUIRED)
find_package(CGAL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(pybind11 REQUIRED)
find_package(pybind11_json REQUIRED)

set(INCLUDE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(src)

set(MOWING_LIBRARIES mowing ${nlohmann_json_LIBRARIES})
set(APPROXIMATION_LIBRARIES approximation ${nlohmann_json_LIBRARIES})

add_subdirectory(apps)
add_subdirectory(tests)