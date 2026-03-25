cmake_minimum_required(VERSION 3.15)

set(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_ROOT ${THIS_DIR})

file(GLOB CMN_SOURCES ${THIS_DIR}/*.cpp)
file(GLOB CMN_INCLUDES ${THIS_DIR}/*.h)

add_subdirectory(format/exe)
