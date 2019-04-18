set(CMAKE_C_FLAGS "-fno-plt -fno-exceptions -fno-unwind-tables -rdynamic -Wno-incompatible-pointer-types")
set(CMAKE_CXX_FLAGS "-fno-plt -fno-rtti -rdynamic")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE PLGS_SRCS plugins/*.c)
add_executable(zxcalc cpm.cxx main.cxx str.cxx ${PLGS_SRCS})
target_link_libraries(zxcalc ${CMAKE_DL_LIBS} -lm)

install(TARGETS zxcalc DESTINATION "${INSTALL_BIN_DIR}")
