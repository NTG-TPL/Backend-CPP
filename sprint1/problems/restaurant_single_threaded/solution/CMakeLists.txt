cmake_minimum_required(VERSION 3.27)
project(restaurant)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED)

add_executable(restaurant src/main.cpp)

target_link_libraries(restaurant boost::boost)