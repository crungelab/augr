cmake_minimum_required(VERSION 3.15)

project(document_test LANGUAGES CXX)
set(THIS ${PROJECT_NAME})
set(THIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB EXE_SOURCES ${THIS_DIR}/*.cpp)
file(GLOB EXE_INCLUDES ${THIS_DIR}/*.h)

add_executable(${THIS} "main.cpp" ${CMN_SOURCES} ${CMN_INCLUDES} ${EXE_SOURCES} ${EXE_INCLUDES})
target_include_directories(${THIS} PRIVATE ${PROJECT_ROOT})

include(${AUGR_CMAKE}/AugrFaust.cmake)
include(${AUGR_CMAKE}/AugrExe.cmake)
include(${AUGR_CMAKE}/AugiteExeApp.cmake)
include(${AUGR_CMAKE}/Pfd.cmake)

USES_AUGR_EXE(${THIS})
USES_AUGR_FAUST(${THIS})
USES_AUGITE_EXE_APP(${THIS})
USES_PFD(${THIS})
