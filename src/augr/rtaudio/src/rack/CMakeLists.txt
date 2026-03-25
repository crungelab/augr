cmake_minimum_required(VERSION 3.14)

#project(AugrRack LANGUAGES CXX)
#set(THIS ${PROJECT_NAME})
#set(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(${AUGR_CMAKE}/AugrRtAudio.cmake)

file(GLOB RACK_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB RACK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB CONTROL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/control/*.cpp)
file(GLOB CONTROL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/control/*.h)

file(GLOB MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cpp)
file(GLOB MODULE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/module/*.h)

# add_library(${THIS} STATIC
#   ${RACK_SRC}
#   ${RACK_INCLUDE}
#   ${CONTROL_SRC}
#   ${CONTROL_INCLUDE}
#   ${MODULE_SRC}
#   ${MODULE_INCLUDE}
# )

target_sources(${THIS}
  PRIVATE
  ${RACK_SRC}
  ${RACK_INCLUDE}
  ${CONTROL_SRC}
  ${CONTROL_INCLUDE}
  ${MODULE_SRC}
  ${MODULE_INCLUDE}
)
#TODO:What's up with byte ambiguous on Windows?
#target_compile_features(${THIS} PUBLIC cxx_std_20)
#target_compile_definitions(${THIS} PRIVATE _HAS_STD_BYTE=0)
#target_include_directories(${THIS} PRIVATE ${THIS_DIR})

USES_AUGR_RTAUDIO(${THIS})

#add_subdirectory(format/exe)