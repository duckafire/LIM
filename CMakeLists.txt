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

set(MASTER  ${CMAKE_BINARY_DIR}/src/master.c)
set(ARULES  ${CMAKE_BINARY_DIR}/src/args/rules)
set(APRINT  ${CMAKE_BINARY_DIR}/src/args/print)
set(CBUFMAN ${CMAKE_BINARY_DIR}/src/core/buf-man)
set(CHEADER ${CMAKE_BINARY_DIR}/src/core/header-lim)
set(CCOMPAC ${CMAKE_BINARY_DIR}/src/core/compaction-process)

add_executable(lim
	${MASTER}
	${ARULES}/check-and-treat.c
	${ARULES}/flag-man.c
	${APRINT}/help.c
	${APRINT}/verbose.c
	${CBUFMAN}/queue.c
	${CHEADER}/read.c
	${CCOMPAC}/check-content/const.c
	${CCOMPAC}/check-content/ident.c
	${CCOMPAC}/process-master.c
	${CCOMPAC}/ident-man.c
	${CCOMPAC}/treat.c
)

# create dinamic header files
set(DIR_CMAKE_H ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake-h)

configure_file(${DIR_CMAKE_H}/version.h.in ${DIR_CMAKE_H}/version.h)
