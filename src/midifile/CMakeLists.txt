cmake_minimum_required(VERSION 3.15)

project(MidiFile LANGUAGES CXX)

set(_ ${MIDIFILE_ROOT})

set(MIDIFILE_HEADERS
    ${_}/include/Binasc.h
    ${_}/include/MidiEvent.h
    ${_}/include/MidiEventList.h
    ${_}/include/MidiFile.h
    ${_}/include/MidiMessage.h
    ${_}/include/Options.h
)

set(MIDIFILE_SRC
    ${_}/src/Binasc.cpp
    ${_}/src/MidiEvent.cpp
    ${_}/src/MidiEventList.cpp
    ${_}/src/MidiFile.cpp
    ${_}/src/MidiMessage.cpp
    ${_}/src/Options.cpp
)

add_library(midifile STATIC ${MIDIFILE_SRC} ${MIDIFILE_HEADERS})