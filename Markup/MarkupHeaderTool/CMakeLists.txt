cmake_minimum_required(VERSION 3.15)

project(KlemmUIHT CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "Source")

file(
	GLOB_RECURSE
	SRCS
	"${SRC_DIR}/**.cpp"
)
add_subdirectory("../MarkupLib" MarkupLib)

add_executable(
	KlemmUIHT
	WIN32
	${SRCS}
)

target_link_libraries(KlemmUIHT PUBLIC "KlemmUIMarkup")
