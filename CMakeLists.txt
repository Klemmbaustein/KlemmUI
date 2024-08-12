cmake_minimum_required(VERSION 3.15)

project(KlemmUI CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "Source")
set(INC_DIR "Include")

file(
	GLOB_RECURSE
	SRCS
	"${SRC_DIR}/**.cpp"
)

set(glew-cmake_BUILD_SHARED OFF)
set(ONLY_LIBS ON)
add_subdirectory("Dependencies/glew-cmake")
add_library(
	KlemmUI STATIC
	${SRCS}
)


target_include_directories(KlemmUI PUBLIC "${INC_DIR}")
target_link_libraries(KlemmUI PUBLIC libglew_static)

add_subdirectory("Examples/HelloWorld")