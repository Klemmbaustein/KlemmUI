cmake_minimum_required(VERSION 3.15)

project(KlemmUIMarkup CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "Source")
set(INC_DIR "Include")

file(
    GLOB_RECURSE
    SRCS
    "${SRC_DIR}/**.cpp"
)
file(
    GLOB_RECURSE
    HEADERS
    "${SRC_DIR}/**.h"
)

add_library(
   KlemmUIMarkup STATIC
   ${SRCS} ${HEADERS}
)
target_include_directories(KlemmUIMarkup PUBLIC "${INC_DIR}")
