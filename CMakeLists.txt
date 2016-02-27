cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_COMPILER_WORKS 1)

project(cppbach)

# =================
# External Projects
# =================
include(ExternalProject)

# midifile

set(MIDIFILE_ROOT ${CMAKE_SOURCE_DIR}/lib/midifile)
set(MIDIFILE_LIB_DIR ${MIDIFILE_ROOT}/src/midifile/lib)
set(MIDIFILE_INCLUDE_DIR ${MIDIFILE_ROOT}/src/midifile/include)

ExternalProject_Add(
    midifile
    PREFIX ${MIDIFILE_ROOT}
    GIT_REPOSITORY "https://github.com/craigsapp/midifile.git"
    GIT_TAG "master"
    BINARY_DIR ${MIDIFILE_ROOT}/src/midifile
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make library
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${MIDIFILE_LIB_DIR}/libmidifile.a)

add_library(libmidifile STATIC IMPORTED)
set_target_properties(libmidifile PROPERTIES IMPORTED_LOCATION ${MIDIFILE_LIB_DIR}/libmidifile.a)
add_dependencies(libmidifile midifile)

# portmidi

set(PORTMIDI_ROOT ${CMAKE_SOURCE_DIR}/lib/portmidi)
set(PORTMIDI_LIB_DIR ${PORTMIDI_ROOT}/src/portmidi-build)
set(PORTMIDI_INCLUDE_DIR ${PORTMIDI_ROOT}/include)

ExternalProject_Add(
    portmidi
    PREFIX ${PORTMIDI_ROOT}
    SVN_REPOSITORY "https://svn.code.sf.net/p/portmedia/code/portmidi/trunk/"
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    UPDATE_COMMAND ""
    PATCH_COMMAND sed -i -e "s/CMAKE_OSX_ARCHITECTURES i386 ppc x86_64/CMAKE_OSX_ARCHITECTURES i386 x86_64/g" CMakeLists.txt
    INSTALL_DIR ${PORTMIDI_INCLUDE_DIR}
    INSTALL_COMMAND cp ../portmidi/pm_common/portmidi.h <INSTALL_DIR> && cp ../portmidi/porttime/porttime.h <INSTALL_DIR>)

add_library(libportmidi STATIC IMPORTED)
set_target_properties(libportmidi PROPERTIES IMPORTED_LOCATION ${PORTMIDI_LIB_DIR}/libportmidi_s.a)
add_dependencies(libportmidi portmidi)

# RtMidi

set(RTMIDI_ROOT ${CMAKE_SOURCE_DIR}/lib/rtmidi)
set(RTMIDI_LIB_DIR ${RTMIDI_ROOT}/lib)
set(RTMIDI_INCLUDE_DIR ${RTMIDI_ROOT}/include)

ExternalProject_Add(
    rtmidi
    PREFIX ${RTMIDI_ROOT}
    URL "https://www.music.mcgill.ca/~gary/rtmidi/release/rtmidi-2.1.1.tar.gz"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND make
    INSTALL_COMMAND make install)

add_library(librtmidi STATIC IMPORTED)
set_target_properties(librtmidi PROPERTIES IMPORTED_LOCATION ${RTMIDI_LIB_DIR}/librtmidi.a)
add_dependencies(librtmidi rtmidi)

# Create dependency of MainTest on googletest

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${MIDIFILE_INCLUDE_DIR})
include_directories(${PORTMIDI_INCLUDE_DIR})

set(SOURCE_FILES src/main.cpp)

add_executable(cppbach ${SOURCE_FILES})

target_link_libraries(cppbach libmidifile libportmidi librtmidi)
