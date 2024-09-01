cmake_minimum_required(VERSION 3.15)

if(DEFINED WIN32)
	add_executable(HelloWorld "main.cpp")
else()
	add_executable(HelloWorld "main.cpp")
endif()

klemmui_resources(HelloWorld "res/")

target_link_libraries(HelloWorld PUBLIC KlemmUI)