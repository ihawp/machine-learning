cmake_minimum_required(VERSION 3.10)
project(YourProjectName CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies using legacy variables
find_package(armadillo CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

# Define the executable
add_executable(regression_model main.cpp)

# Define include path for header-only mlpack
find_path(MLPACK_INCLUDE_DIRS "mlpack.hpp")
target_include_directories(regression_model PRIVATE ${MLPACK_INCLUDE_DIRS})

# Link all required libraries using legacy variables
target_link_libraries(regression_model PRIVATE 
    ${ARMADILLO_LIBRARIES}
    ${CEREAL_LIBRARIES}
)