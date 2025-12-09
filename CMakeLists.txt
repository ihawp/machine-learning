cmake_minimum_required(VERSION 3.10)
project(YourProjectName CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(armadillo CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

find_path(MLPACK_INCLUDE_DIRS "mlpack.hpp")
if(NOT MLPACK_INCLUDE_DIRS)
    message(FATAL_ERROR "mlpack.hpp not found! Check vcpkg installation.")
endif()

add_executable(regression_model main.cpp)

target_link_libraries(regression_model PRIVATE 
    armadillo
    cereal::cereal
)

# Add the found include directory to the target
target_include_directories(regression_model PRIVATE ${MLPACK_INCLUDE_DIRS})