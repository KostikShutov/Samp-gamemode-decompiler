project(decompiler)
cmake_minimum_required(VERSION 3.9)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

set(VERSION_MAJOR 3)
set(VERSION_MINOR 10)
set(VERSION_BUILD 6)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD})
set(VERSION_STR ${VERSION})
math(EXPR VERSION_INT "${VERSION_MAJOR} << 8 | ${VERSION_MINOR}")

# check for optional include files
include(CheckIncludeFile)
check_include_file("unistd.h" HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
  add_definitions(-DHAVE_UNISTD_H)
endif()
check_include_file("inttypes.h" HAVE_INTTYPES_H)
if(HAVE_INTTYPES_H)
  add_definitions(-DHAVE_INTTYPES_H)
endif()
check_include_file("stdint.h" HAVE_STDINT_H)
if(HAVE_STDINT_H)
  add_definitions(-DHAVE_STDINT_H)
endif()
check_include_file("alloca.h" HAVE_ALLOCA_H)
if(HAVE_ALLOCA_H)
  add_definitions(-DHAVE_ALLOCA_H)
endif()
check_include_file("endian.h" HAVE_ENDIAN_H)
if(HAVE_ENDIAN_H)
  add_definitions(-DHAVE_ENDIAN_H)
endif()

# check for optional library functions
include(CheckFunctionExists)
check_function_exists(strlcpy HAVE_STRLCPY)
if(HAVE_STRLCPY)
  add_definitions(-DHAVE_STRLCPY)
endif()
check_function_exists(strlcat HAVE_STRLCAT)
if(HAVE_STRLCAT)
  add_definitions(-DHAVE_STRLCAT)
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
  if(MSVC LESS 1900)
    # MSVC 2013 and below don't support the "inline" keyword
    add_definitions(-Dinline=__inline)
  endif()
endif()

if(UNIX)
  add_definitions(-DLINUX -DENABLE_BINRELOC -g)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../linux)
  link_libraries(pthread)
endif()

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# The Decompiler
set(DECOMPILER_SRCS
  components/analyze.cpp
  components/generate.cpp
  components/load.cpp
  parse.cpp
  decompiler.c
  funcs.c
  main.cpp
  ../amx/amxdbg.c)
add_executable(main ${DECOMPILER_SRCS})
if(UNIX)
  target_link_libraries(main dl)
endif()
