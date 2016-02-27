cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_COMPILER_WORKS 1)

project(cppbach)

include(ExternalProject)

set(MIDIFILE_ROOT ${CMAKE_SOURCE_DIR}/lib/midifile)
set(MIDIFILE_LIB_DIR ${MIDIFILE_ROOT}/src/midifile/lib)
set(MIDIFILE_INCLUDE_DIR ${MIDIFILE_ROOT}/src/midifile/include)

ExternalProject_Add(
    midifile
    PREFIX ${MIDIFILE_ROOT}
    GIT_REPOSITORY "https://github.com/craigsapp/midifile.git"
    GIT_TAG "master"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    BINARY_DIR ${MIDIFILE_ROOT}/src/midifile
    SOURCE_DIR ${MIDIFILE_ROOT}/src/midifile
    CONFIGURE_COMMAND make library
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${MIDIFILE_LIB_DIR}/libmidifile.a)

add_library(libmidifile STATIC IMPORTED)
set_target_properties(libmidifile PROPERTIES IMPORTED_LOCATION ${MIDIFILE_LIB_DIR}/libmidifile.a)
add_dependencies(libmidifile midifile)

set(PORTMIDI_ROOT ${CMAKE_SOURCE_DIR}/lib/portmidi)
set(PORTMIDI_LIB_DIR ${PORTMIDI_ROOT}/src/portmidi-build)
set(PORTMIDI_INCLUDE_DIR ${PORTMIDI_ROOT}/src/portmidi-build/include)

ExternalProject_Add(
    portmidi
    PREFIX ${PORTMIDI_ROOT}
    SVN_REPOSITORY "http://svn.code.sf.net/p/portmedia/code/portmidi/trunk/"
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    PATCH_COMMAND sed -i -e "s/CMAKE_OSX_ARCHITECTURES i386 ppc x86_64/CMAKE_OSX_ARCHITECTURES i386 x86_64/g" CMakeLists.txt
    INSTALL_DIR ${PORTMIDI_INCLUDE_DIR}
    INSTALL_COMMAND cp ../portmidi/pm_common/portmidi.h ${PORTMIDI_INCLUDE_DIR}/ && cp ../portmidi/porttime/porttime.h ${PORTMIDI_INCLUDE_DIR}/
    )

add_library(libportmidi STATIC IMPORTED)
set_target_properties(libportmidi PROPERTIES IMPORTED_LOCATION ${PORTMIDI_LIB_DIR}/libportmidi_s.a)
add_dependencies(libportmidi portmidi)

# Create dependency of MainTest on googletest

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${MIDIFILE_INCLUDE_DIR})
include_directories(${PORTMIDI_INCLUDE_DIR})

set(SOURCE_FILES src/main.cpp)

add_executable(cppbach ${SOURCE_FILES})

target_link_libraries(cppbach libmidifile libportmidi)
