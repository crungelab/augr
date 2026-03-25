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

  # Remember we defined a rule for this path
  set_property(GLOBAL APPEND PROPERTY FAUST_OUTPUTS "${_out}")

  # Return to caller
  set(GENERATED_HEADER "${_out}" PARENT_SCOPE)
endfunction()

# ---- Declare your DSPs exactly once -----------------------------------------

# Analysis
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/analysis/dbmeter.dsp"
  "DbMeterDsp"
  "dbmeter_dsp.h"
)
list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

# Phasing
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/phasing/phaser.dsp"
  "PhaserDsp"
  "phaser_dsp.h"
)
list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/phasing/flanger.dsp"
  "FlangerDsp"
  "flanger_dsp.h"
)
list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

# Physical Modeling
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/physicalModeling/frenchBell.dsp"
  "FrenchBellDsp"
  "frenchbell_dsp.h"
)
list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

# Generator
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/generator/osc.dsp"
  "OscDsp"
  "osc_dsp.h"
)
list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

# Reverb
faust_generate_header_once(
  "${FAUST_EXAMPLES_DIR}/reverb/freeverb.dsp"
  "FreeverbDsp"
  "freeverb_dsp.h"
)
list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

# Add more DSPs here with unique OUT_BASENAME values
# faust_generate_header_once("${CMAKE_CURRENT_SOURCE_DIR}/dsp/organ.dsp" "OrganDsp" "organ_dsp.h")
# list(APPEND _ALL_FAUST_HEADERS "${GENERATED_HEADER}")

# Single aggregate target that depends on *all* headers
add_custom_target(faust_headers ALL DEPENDS ${_ALL_FAUST_HEADERS})

include(${AUGR_CMAKE}/AugrCore.cmake)

set(THIS fausty_dsp)

# Consumers
add_library(${THIS} STATIC library.cpp ${_ALL_FAUST_HEADERS})
add_dependencies(${THIS} faust_headers)
target_include_directories(${THIS} PUBLIC "${FAUST_GEN_DIR}")
USES_AUGR_CORE(${THIS})
