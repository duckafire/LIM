cmake_minimum_required(VERSION 3.10)

# define the executable destine
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-alpha)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

project(lim
	VERSION      ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
	DESCRIPTION  "Lua library compactor"
	LANGUAGES    C
)

# set files
add_executable(lim
	${CMAKE_BINARY_DIR}/src/master/main.c
	${CMAKE_BINARY_DIR}/src/args/rules/check-and-treat.c
	${CMAKE_BINARY_DIR}/src/args/rules/flag-man.c
	${CMAKE_BINARY_DIR}/src/args/print/help.c
	${CMAKE_BINARY_DIR}/src/args/print/verbose.c
	#${CMAKE_BINARY_DIR}/src/master/*
	#${CMAKE_BINARY_DIR}/src/master/*
)

# create dinamic header files
set(DIR_CMAKE_H ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake-h)

configure_file(${DIR_CMAKE_H}/version.h.in ${DIR_CMAKE_H}/version.h)

# set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# "libm" is not auto-implemented, like "libc"
# target_link_libraries(lim m)
