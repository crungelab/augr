include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

set(VST3SDK_DIR ${AUGR_DEPOT}/vst3sdk)
set(VST3SDK_LIB ${AUGR_DEPOT}/vst3sdk/build/lib/Debug)
set(VST3SDK_LIB_PLUGINTERFACES ${VST3SDK_LIB}/pluginterfaces.lib)
set(VST3SDK_LIB_BASE ${VST3SDK_LIB}/base.lib)
set(VST3SDK_LIB_SDK ${VST3SDK_LIB}/sdk.lib)
set(VST3SDK_LIB_SDK_COMMON ${VST3SDK_LIB}/sdk_common.lib)

function(USES_VST3SDK THIS)
  set(public_sdk_SOURCE_DIR ${AUGR_DEPOT}/vst3sdk/public.sdk)
  smtg_target_add_library_main(${THIS}) #from vst3sdk
  
  USES_STANDARD(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${VST3SDK_DIR}
  )
  target_link_libraries(${THIS} PRIVATE
    sdk_common
    sdk
    base
    pluginterfaces
    #${VST3SDK_LIB_PLUGINTERFACES}
    #${VST3SDK_LIB_BASE}
    #${VST3SDK_LIB_SDK}
    #${VST3SDK_LIB_SDK_COMMON}
  )
endfunction()
