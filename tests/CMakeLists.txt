cmake_minimum_required(VERSION 3.15)

include(${AUGR_CMAKE}/AugrCore.cmake)
include(${AUGR_CMAKE}/ExeApp.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)

add_subdirectory(serialize)
add_subdirectory(document)