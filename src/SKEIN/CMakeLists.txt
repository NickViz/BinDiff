# Author: Nikolai Vorontsov
# Copyright (c) 2010-2020 VorontSOFT
cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(SKEIN LANGUAGES C)

file(GLOB SKEIN_HEADERS *.h)
file(GLOB SKEIN_SOURCES *.c)
add_library(${PROJECT_NAME} STATIC ${SKEIN_HEADERS} ${SKEIN_SOURCES})

# Add asm files when possible
if(${USE_ASM})
  if("${CMAKE_GENERATOR}" MATCHES "Visual Studio")  # Only for Visual Studio ML assembler
    if("${CMAKE_GENERATOR_PLATFORM}" MATCHES "x64") # 64-bit
      enable_language(ASM_MASM)
      target_sources(${PROJECT_NAME} PRIVATE skein_block_x64.asm)
	  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DSKEIN_USE_ASM=1792") #256+512+1024
      message("skein_block_x64.asm was added")
    elseif("${CMAKE_GENERATOR_PLATFORM}" MATCHES "ARM") # ARM/ARM64, do nothing
    else() # 32-bit, can be Win32 or empty
      enable_language(ASM_MASM)
      target_sources(${PROJECT_NAME} PRIVATE skein_block_xmm32.asm)
	  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DSKEIN_USE_ASM=1792")
	  set(CMAKE_ASM_MASM_FLAGS "${CMAKE_ASM_MASM_FLAGS} /safeseh")
      message("skein_block_xmm32.asm was added")
    endif()
  endif()
endif()

if(${STATIC_RUNTIME})
  set_property(TARGET ${PROJECT_NAME} PROPERTY
               MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
