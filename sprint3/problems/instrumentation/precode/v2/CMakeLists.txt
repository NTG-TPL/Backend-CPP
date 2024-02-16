cmake_minimum_required(VERSION 3.22)
project(events2dot)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

add_executable(${PROJECT_NAME} main.cpp
        config.h
        config.cpp
        graph.h
        graph.cpp
        binarytree.h
        binarytree.cpp
        dotgen.h
        dotgen.cpp
        readfile.h
        readfile.cpp)
