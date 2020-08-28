# Author: Nikolai Vorontsov
# Copyright (c) 2010-2020 VorontSOFT
cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(BinDiff VERSION 1.8.0.0 LANGUAGES CXX)

file(GLOB BINDIFF_HEADERS *.h)
file(GLOB BINDIFF_SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${BINDIFF_HEADERS} ${BINDIFF_SOURCES})

add_subdirectory(SKEIN)

target_include_directories(${PROJECT_NAME} PRIVATE SKEIN)
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

if(WIN32)
  option(USE_ASM "Use MS assembler to speedup SKEIN blocks calculation" True)
  option(MULTI_THREADED "Build multithreaded application" False)
  option(STATIC_RUNTIME "Use static runtime library" True)
  target_sources(${PROJECT_NAME} PRIVATE BinDiff.rc)
  if(${STATIC_RUNTIME})
    set_property(TARGET ${PROJECT_NAME} PROPERTY
                 MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()	
else()
  set(USE_ASM False)
  set(MULTI_THREADED False)
  set(STATIC_RUNTIME False)
endif()

if(${MULTI_THREADED})
  message("Multithreaded app")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DMULTITHREADED")
endif()

target_link_libraries(${PROJECT_NAME} SKEIN)
