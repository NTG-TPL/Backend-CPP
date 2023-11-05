cmake_minimum_required(VERSION 3.11)
project(Hello CXX)

set(CMAKE_CXX_STANDARD 20)
include(build/conan_toolchain.cmake)

find_package(Boost 1.83.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(hello src/main.cpp)
target_link_libraries(hello PRIVATE Threads::Threads)
