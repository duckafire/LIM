cmake_minimum_required(VERSION 3.10)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-alpha)

project(lim
	VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
	DESCRIPTION "Lua library compactor"
	LANGUAGES C
)

configure_file(version.h.in ./src/version.h)

add_executable(lim
	${SRC_DIR}/check-flags.c
	${SRC_DIR}/compaction-process.c
	${SRC_DIR}/content-treatment.c
	${SRC_DIR}/main.c
	${SRC_DIR}/tools.c
	${SRC_DIR}/errors.c
	${SRC_DIR}/informations.c
	${SRC_DIR}/buffers.c
	${SRC_DIR}/header.c
	${SRC_DIR}/memory-manipulation.c
	${SRC_DIR}/stage-product.c
)

# on the contrary of "(lib)c", "(lib)m" is not
# auto-implemented by cmake, because of this, its
# use must be specified (after `add_executable`)
target_link_libraries(lim m)
