cmake_minimum_required(VERSION 3.14)

project(Augite LANGUAGES CXX)
set(THIS ${PROJECT_NAME})

file(GLOB AUGITE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB AUGITE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC
  ${AUGITE_SRC}
  ${AUGITE_INCLUDE}
)

#include(${AUGR_CMAKE}/SpdLog.cmake)
#USES_SPDLOG(${THIS})
add_subdirectory(shell)
add_subdirectory(vst3)
#add_subdirectory(rack)
add_subdirectory(widget)
add_subdirectory(view)
add_subdirectory(app)
add_subdirectory(system)

