cmake_minimum_required(VERSION 3.15)

if(DEFINED WIN32)
	add_executable(Translation WIN32 "main.cpp")
else()
	add_executable(Translation "main.cpp")
endif()

klemmui_resources(Translation "../res/")
klemmui_markup(Translation "ui/" "elements/")

target_link_libraries(Translation PUBLIC KlemmUI)