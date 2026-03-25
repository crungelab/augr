include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/AugrCore.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/RtAudio.cmake)

function(USES_AUGR_RTAUDIO THIS)
  USES_AUGR_CORE(${THIS})
  USES_RTAUDIO(${THIS})
  target_include_directories(${THIS} PRIVATE ${AUGR_RTAUDIO_ROOT}/include)
  target_compile_definitions(${THIS} PRIVATE "NOMINMAX")
endfunction()

function (LINKS_AUGR_RTAUDIO THIS)
  USES_AUGR_RTAUDIO(${THIS})
  target_link_libraries(${THIS} PRIVATE AugrRtAudio)
endfunction()