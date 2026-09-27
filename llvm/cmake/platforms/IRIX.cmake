# Toolchain file for building LLVM (or anything else) to run on IRIX 6.5,
# with a clang that targets IRIX -- one built from this tree:
#
#   cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=<llvm>/cmake/platforms/IRIX.cmake \
#         -DIRIX_SYSROOT=<IRIX root: usr/include, usr/lib32, lib32> \
#         -DIRIX_TOOLCHAIN=<prefix with bin/clang, lib32/libc++, ...> ...
#
# IRIX_ABI picks n32 (the default) or 64; IRIX_CPU the instruction set, mips3
# by default so the result runs on every MIPS III machine (an R4400 Indy
# included); IRIX_TUNE the processor to schedule for.
#
# CMake no longer knows IRIX, so IRIX/Platform/IRIX.cmake supplies the little
# it needs.

set(CMAKE_SYSTEM_NAME IRIX)
set(CMAKE_SYSTEM_VERSION 6.5)
set(CMAKE_SYSTEM_PROCESSOR mips64)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/IRIX")

foreach(var IRIX_SYSROOT IRIX_TOOLCHAIN)
  if(NOT ${var})
    message(FATAL_ERROR "${var} is required")
  endif()
endforeach()
# try_compile projects get a fresh cache: pass these through.
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
     IRIX_SYSROOT IRIX_TOOLCHAIN IRIX_ABI IRIX_CPU IRIX_TUNE)
set(IRIX_ABI "${IRIX_ABI}" CACHE STRING "n32 or 64")
if(NOT IRIX_ABI)
  set(IRIX_ABI n32)
endif()
if(NOT IRIX_CPU)
  set(IRIX_CPU mips3)
endif()

set(triple mips64-sgi-irix6.5)
set(CMAKE_C_COMPILER "${IRIX_TOOLCHAIN}/bin/clang")
set(CMAKE_CXX_COMPILER "${IRIX_TOOLCHAIN}/bin/clang++")
set(CMAKE_ASM_COMPILER "${IRIX_TOOLCHAIN}/bin/clang")
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_ASM_COMPILER_TARGET ${triple})
set(CMAKE_AR "${IRIX_TOOLCHAIN}/bin/llvm-ar" CACHE FILEPATH "")
set(CMAKE_RANLIB "${IRIX_TOOLCHAIN}/bin/llvm-ranlib" CACHE FILEPATH "")
set(CMAKE_SYSROOT "${IRIX_SYSROOT}")

set(flags "-mabi=${IRIX_ABI} -march=${IRIX_CPU}")
if(IRIX_TUNE)
  string(APPEND flags " -mtune=${IRIX_TUNE}")
endif()
set(CMAKE_C_FLAGS_INIT "${flags}")
set(CMAKE_CXX_FLAGS_INIT "${flags}")
set(CMAKE_ASM_FLAGS_INIT "${flags}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
