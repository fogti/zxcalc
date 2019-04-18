cmake_minimum_required(VERSION 3.7)
project(zxcalc)

set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# check for -lm
include(CheckSymbolExists)

if(NOT POW_FUNCTION_EXISTS AND NOT NEED_LINKING_AGAINST_LIBM)
  check_symbol_exists(pow "math.h" POW_FUNCTION_EXISTS)
  if(NOT POW_FUNCTION_EXISTS)
    unset(POW_FUNCTION_EXISTS CACHE)
    list(APPEND CMAKE_REQUIRED_LIBRARIES m)
    check_symbol_exists(pow "math.h" POW_FUNCTION_EXISTS)
    if(POW_FUNCTION_EXISTS)
      set(NEED_LINKING_AGAINST_LIBM True CACHE BOOL "" FORCE)
    else()
      message(FATAL_ERROR "Failed making the pow() function available")
    endif()
  endif()
endif()
# end check for -lm

find_package(Libedit)
if(LIBEDIT_FOUND)
  find_package(Curses REQUIRED)
endif()

configure_file(zxcalc_config.h.in zxcalc_config.h @ONLY)
include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(src)
