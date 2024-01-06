cmake_minimum_required(VERSION 3.22)
project(restaurant)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost 1.83.0 REQUIRED)

add_executable(restaurant src/main.cpp)

target_link_libraries(restaurant Boost::boost)