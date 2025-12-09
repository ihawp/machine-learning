cmake_minimum_required(VERSION 3.10)
project(YourProjectName CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# These packages work correctly with CONFIG mode and modern targets:
find_package(armadillo CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

# --- CORRECTED MLPACK HANDLING ---
# Use find_path() for header-only mlpack as suggested by vcpkg
find_path(MLPACK_INCLUDE_DIRS "mlpack.hpp")
if(NOT MLPACK_INCLUDE_DIRS)
    message(FATAL_ERROR "mlpack.hpp not found! Check vcpkg installation.")
endif()

add_executable(regression_model main.cpp)

target_link_libraries(regression_model PRIVATE 
    armadillo
    cereal::cereal
    # mlpack does not provide a target, link its include dirs manually
)

# Add the found include directory to the target
target_include_directories(regression_model PRIVATE ${MLPACK_INCLUDE_DIRS})