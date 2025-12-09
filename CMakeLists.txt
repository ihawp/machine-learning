cmake_minimum_required(VERSION 3.10)
project(YourProjectName CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(armadillo CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(mlpack CONFIG REQUIRED)

add_executable(regression_model main.cpp)

target_link_libraries(regression_model PRIVATE 
    armadillo::armadillo
    cereal::cereal
    mlpack::mlpack
)