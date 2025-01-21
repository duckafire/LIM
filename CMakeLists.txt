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

set(MAIN ${CMAKE_BINARY_DIR}/src/main.c)
set(ARULES ${CMAKE_BINARY_DIR}/src/args/rules)
set(APRINT ${CMAKE_BINARY_DIR}/src/args/print)
set(CORE   ${CMAKE_BINARY_DIR}/src/core)
set(CCHECK ${CMAKE_BINARY_DIR}/src/core/check-content)
set(CTOOLS ${CMAKE_BINARY_DIR}/src/core/tools)

add_executable(lim
	${MAIN}
	${ARULES}/check-and-treat.c
	${ARULES}/flag-man.c
	${APRINT}/help.c
	${APRINT}/verbose.c
	${CORE}/ident-man.c
	${CORE}/process-master.c
	${CORE}/treat.c
	${CCHECK}/const.c
	${CCHECK}/ident.c
	${CTOOLS}/queue.c
	${CTOOLS}/lim-global-variables.c
	${CTOOLS}/read-header-lim.c
	${CTOOLS}/string-plus.c
)

# create dinamic header files
set(DIR_CMAKE_H ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake-h)

configure_file(${DIR_CMAKE_H}/version.h.in ${DIR_CMAKE_H}/version.h)
