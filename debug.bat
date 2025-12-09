@echo off
REM Build the c++ project.
cmake --preset=local-env
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C://vcpkg/scripts/buildsystems/vcpkg.cmake