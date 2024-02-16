cmake_minimum_required(VERSION 3.22)
project(events2dot)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        config.cpp
        config.h
        dotgen.cpp
        dotgen.h
        graph.cpp
        graph.h
        main.cpp
        readfile.cpp
        readfile.h)
