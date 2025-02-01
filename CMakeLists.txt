cmake_minimum_required(VERSION 3.10)


option(DEBUG_VERSION ON "Turn on GDB bug flag ('-g') + move executable to '../tests/'")

if(DEBUG_VERSION)
	set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../tests/bin/)
else(DEBUG_VERSION)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/)
endif(DEBUG_VERSION)


set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

project(lim
	VERSION      ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
	DESCRIPTION  "Lua library compactor"
	LANGUAGES    C
)


file(GLOB MAIN       ${CMAKE_BINARY_DIR}/../src/*.c)
file(GLOB ARGS_PRINT ${CMAKE_BINARY_DIR}/../src/args/print/*.c)
file(GLOB ARGS_RULES ${CMAKE_BINARY_DIR}/../src/args/rules/*.c)
file(GLOB CORE       ${CMAKE_BINARY_DIR}/../src/core/*.c)
file(GLOB CORE_TOOLS ${CMAKE_BINARY_DIR}/../src/core/tools/*.c)
FILE(GLOB CORE_CKCTT ${CMAKE_BINARY_DIR}/../src/core/check-content/*.c)

add_executable(lim ${MAIN} ${ARGS_PRINT} ${ARGS_RULES} ${CORE} ${CORE_TOOLS} ${CORE_CKCTT})


set(HEADER_DIR ${CMAKE_BINARY_DIR}/../src/cmake-h)

configure_file(${HEADER_DIR}/version.h.in ${HEADER_DIR}/version.h)
