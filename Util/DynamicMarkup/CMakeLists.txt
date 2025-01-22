cmake_minimum_required(VERSION 3.15)

file(
    GLOB_RECURSE
    SRCS
    "Source/**.cpp"
)
file(
    GLOB_RECURSE
    HEADERS
    "Source/**.h"
)
file(
    GLOB_RECURSE
    INCLUDES
    "Include/**.h"
)

add_library(
   KuiDynamicMarkup STATIC
   ${SRCS} ${HEADERS} ${INCLUDES}
)

target_include_directories(KuiDynamicMarkup PUBLIC "Include")

target_link_libraries(KuiDynamicMarkup PUBLIC KlemmUIMarkup)
target_link_libraries(KuiDynamicMarkup PUBLIC KlemmUI)