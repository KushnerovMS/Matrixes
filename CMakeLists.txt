cmake_minimum_required(VERSION 3.16) 

project(Matrix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE Matrix_src
    src/Matrix/*.h src/Matrix/*.cpp)

add_executable(Matrix src/main.cpp ${Matrix_src})

#add_executable(TestOfEfficiency src/TestOfEfficiency.cpp src/ARC.hpp src/IdealCache.hpp src/LRUC.hpp)

#add_subdirectory(tests)
