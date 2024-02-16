cmake_minimum_required(VERSION 3.22)
project(events2dot)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp
        dotgen.cpp
        binarytree.cpp
        config.cpp
        graph.cpp
        readfile.cpp)
