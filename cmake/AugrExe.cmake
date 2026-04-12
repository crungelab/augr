include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrCore.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtMidi.cmake)

function(INTERNAL_AUGR_EXE THIS)
  USES_AUGR_CORE(${THIS})
  USES_RTAUDIO(${THIS})
  USES_RTMIDI(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_EXE_ROOT}/include)
endfunction()

function(USES_AUGR_EXE THIS)
  INTERNAL_AUGR_EXE(${THIS})
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
  target_link_libraries(${THIS} PRIVATE AugrExe)
endfunction()
