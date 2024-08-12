cmake_minimum_required(VERSION 3.15)


if(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	add_executable(HelloWorld WIN32 "main.cpp")
else()
	add_executable(HelloWorld "main.cpp")
endif()

target_link_libraries(HelloWorld PUBLIC KlemmUI)