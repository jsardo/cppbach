cmake_minimum_required(VERSION 3.2)
project(cppbach)


set(MIDIFILE_ROOT ${CMAKE_SOURCE_DIR}/lib/midifile)
set(MIDIFILE_LIB_DIR ${MIDIFILE_ROOT}/src/midifile/lib)
set(MIDIFILE_INCLUDE_DIR ${MIDIFILE_ROOT}/src/midifile/include)

include(ExternalProject)

ExternalProject_Add(midifile
  PREFIX ${MIDIFILE_ROOT}
  GIT_REPOSITORY "https://github.com/craigsapp/midifile.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BINARY_DIR ${MIDIFILE_ROOT}/src/midifile
  SOURCE_DIR ${MIDIFILE_ROOT}/src/midifile
  INSTALL_DIR ${MIDIFILE_ROOT}/bin
  CONFIGURE_COMMAND make library
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${MIDIFILE_LIB_DIR}/libmidifile.a)

add_library(libmidifile STATIC IMPORTED)
set_target_properties(libmidifile PROPERTIES IMPORTED_LOCATION ${MIDIFILE_LIB_DIR}/libmidifile.a)
add_dependencies(libmidifile midifile)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${MIDIFILE_INCLUDE_DIR})

set(SOURCE_FILES src/main.cpp)

add_executable(cppbach ${SOURCE_FILES})

target_link_libraries(cppbach libmidifile)
