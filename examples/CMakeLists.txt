cmake_minimum_required(VERSION 3.15)

include(${AUGR_CMAKE}/RtAudio.cmake)

include(${AUGR_CMAKE}/AugrCore.cmake)
include(${AUGR_CMAKE}/ExeApp.cmake)
include(${AUGR_CMAKE}/ImNodes.cmake)

add_executable(rtplaysaw rtplaysaw.cpp)
USES_RTAUDIO(rtplaysaw)

add_executable(xtbuffer xtbuffer.cpp ${AUGR_HEADERS})
USES_RTAUDIO(xtbuffer)
USES_AUGR_CORE(xtbuffer)

add_executable(ports ports.cpp ${AUGR_HEADERS})
USES_RTAUDIO(ports)
USES_AUGR_CORE(ports)

add_executable(app app.cpp ${AUGR_HEADERS})
USES_EXE_APP(app)

add_executable(graph graph.cpp ${AUGR_HEADERS})
USES_EXE_APP(graph)
USES_IMNODES(graph)

add_subdirectory(panner)

add_subdirectory(osc)

add_subdirectory(frenchbell)

add_subdirectory(chain)

add_subdirectory(multi)

add_subdirectory(bubble)

add_subdirectory(daw)