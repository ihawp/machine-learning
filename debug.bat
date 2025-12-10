@echo off
REM Build the c++ project.
cmake --preset=local-env
cmake --build build