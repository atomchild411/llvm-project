# IRIX 6.5, for CMake, which dropped its own support: an ELF Unix whose
# runtime linker, rld, reads DT_RPATH (not DT_RUNPATH, and no $ORIGIN) and
# whose dlopen lives in libc.
include(Platform/UnixPaths)

foreach(lang C CXX ASM)
  set(CMAKE_SHARED_LIBRARY_${lang}_FLAGS "-fPIC")
  set(CMAKE_SHARED_LIBRARY_CREATE_${lang}_FLAGS "-shared")
  set(CMAKE_SHARED_LIBRARY_SONAME_${lang}_FLAG "-Wl,-soname,")
  set(CMAKE_SHARED_LIBRARY_RUNTIME_${lang}_FLAG "-Wl,-rpath,")
  set(CMAKE_SHARED_LIBRARY_RUNTIME_${lang}_FLAG_SEP ":")
  set(CMAKE_SHARED_LIBRARY_RPATH_LINK_${lang}_FLAG "-Wl,-rpath-link,")
  set(CMAKE_EXE_EXPORTS_${lang}_FLAG "-Wl,--export-dynamic")
endforeach()
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_SHARED_MODULE_PREFIX "lib")
set(CMAKE_SHARED_MODULE_SUFFIX ".so")
set(CMAKE_DL_LIBS "")
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
# n32 libraries are in lib32, n64 ones in lib64.
if(IRIX_ABI STREQUAL "64")
  set(CMAKE_LIBRARY_ARCHITECTURE_REGEX "")
  list(APPEND CMAKE_SYSTEM_LIBRARY_PATH /usr/lib64 /lib64)
else()
  list(APPEND CMAKE_SYSTEM_LIBRARY_PATH /usr/lib32 /lib32)
endif()
