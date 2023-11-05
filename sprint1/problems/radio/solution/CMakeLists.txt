cmake_minimum_required(VERSION 3.11)

project(Radio CXX)
set(CMAKE_CXX_STANDARD 20)
include(build/conan_toolchain.cmake)

# Ищем Boost версии 1.83
find_package(Boost 1.83.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(miniaudio 0.11.17  REQUIRED)

# Под Windows нужно определить макрос NOMINMAX для корректной работы при включении библиотеки minisound
if(WIN32)
  add_definitions(-DNOMINMAX)
endif()

add_executable(radio src/main.cpp src/audio.h)
target_link_libraries(radio PRIVATE miniaudio::miniaudio Threads::Threads)
