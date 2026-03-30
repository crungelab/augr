set(THIS AugrFaustLibrary)

# One place to collect all outputs
set(FAUST_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/faust")
file(MAKE_DIRECTORY "${FAUST_GEN_DIR}")

# Find Faust once
find_program(FAUST_EXECUTABLE NAMES faust)
if(NOT FAUST_EXECUTABLE)
  message(FATAL_ERROR "Faust compiler not found")
endif()

# Where Faust includes live (your -I)
set(FAUST_INCLUDE_DIRS "${AUGR_ROOT}/depot/faust")
set(FAUST_EXAMPLES_DIR "${AUGR_ROOT}/depot/faust/examples")

# Collect outputs in a variable (not via per-file custom targets)
set(_ALL_FAUST_HEADERS "")
set_property(GLOBAL PROPERTY FAUST_OUTPUTS "")
set_property(GLOBAL PROPERTY FAUST_ALL_HEADERS "")
set_property(GLOBAL PROPERTY FAUST_HEADER_TARGETS "")

function(faust_generate_header_once DSP CLASS_NAME OUT_BASENAME)
  # Compute absolute, names, and output
  get_filename_component(_dsp_abs "${DSP}" ABSOLUTE)
  set(_out "${FAUST_GEN_DIR}/${OUT_BASENAME}")

  # If we already registered this exact OUTPUT, don't add another rule
  get_property(_already GLOBAL PROPERTY FAUST_OUTPUTS)
  if(_already)
    list(FIND _already "${_out}" _idx)
  else()
    set(_idx -1)
  endif()
  if(_idx GREATER -1)
    # Just expose it to caller—rule already exists
    string(SHA1 _out_hash "${_out}")
    set(_hdr_target "faust_header_${_out_hash}")
    set_property(GLOBAL APPEND PROPERTY FAUST_HEADER_TARGETS "${_hdr_target}")
    set_property(GLOBAL APPEND PROPERTY FAUST_ALL_HEADERS "${_out}")
    set(GENERATED_HEADER "${_out}" PARENT_SCOPE)
    return()
  endif()

  # Build -I args
  set(_I_ARGS "")
  foreach(_I IN LISTS FAUST_INCLUDE_DIRS)
    list(APPEND _I_ARGS -I "${_I}")
  endforeach()

  add_custom_command(
    OUTPUT  "${_out}"
    COMMAND "${FAUST_EXECUTABLE}"
            "${_dsp_abs}"
            -o "${_out}"
            ${_I_ARGS}
            -cn "${CLASS_NAME}"
            -scn "augr::FaustDsp"
    DEPENDS "${_dsp_abs}"
    COMMENT "Faust: ${DSP} -> ${OUT_BASENAME}"
    VERBATIM
  )

  # Wrap each generated header in its own target so cross-directory deps are reliable.
  string(SHA1 _out_hash "${_out}")
  set(_hdr_target "faust_header_${_out_hash}")
  add_custom_target("${_hdr_target}" DEPENDS "${_out}")

  # Remember we defined a rule for this path
  set_property(GLOBAL APPEND PROPERTY FAUST_OUTPUTS "${_out}")
  set_property(GLOBAL APPEND PROPERTY FAUST_ALL_HEADERS "${_out}")
  set_property(GLOBAL APPEND PROPERTY FAUST_HEADER_TARGETS "${_hdr_target}")

  # Return to caller
  set(GENERATED_HEADER "${_out}" PARENT_SCOPE)
endfunction()

# ---- Declare your DSPs exactly once -----------------------------------------

# Generator
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/generator/osc.dsp"
  "OscDsp"
  "osc_dsp.h"
)

# Reverb
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/reverb/freeverb.dsp"
  "FreeverbDsp"
  "freeverb_dsp.h"
)

# Add more DSPs here with unique OUT_BASENAME values
# faust_generate_header_once("${CMAKE_CURRENT_SOURCE_DIR}/dsp/organ.dsp" "OrganDsp" "organ_dsp.h")
# Headers are auto-registered by faust_generate_header_once.

include(${AUGR_CMAKE}/AugrCore.cmake)

# Consumers
add_library(${THIS} STATIC library.cpp)
target_include_directories(${THIS} PUBLIC "${FAUST_GEN_DIR}")
USES_AUGR_CORE(${THIS})

add_subdirectory(analysis)
add_subdirectory(gameaudio)
add_subdirectory(phasing)
add_subdirectory(physicalModeling)

get_property(_ALL_FAUST_HEADERS GLOBAL PROPERTY FAUST_ALL_HEADERS)
if(_ALL_FAUST_HEADERS)
  list(REMOVE_DUPLICATES _ALL_FAUST_HEADERS)
endif()

get_property(_FAUST_HEADER_TARGETS GLOBAL PROPERTY FAUST_HEADER_TARGETS)
if(_FAUST_HEADER_TARGETS)
  list(REMOVE_DUPLICATES _FAUST_HEADER_TARGETS)
endif()

# Single aggregate target that depends on *all* headers
add_custom_target(faust_headers ALL DEPENDS ${_FAUST_HEADER_TARGETS})
add_dependencies(${THIS} faust_headers)
