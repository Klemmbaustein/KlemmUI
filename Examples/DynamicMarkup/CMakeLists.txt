cmake_minimum_required(VERSION 3.15)

if(DEFINED WIN32)
	add_executable(DynamicMarkupExample WIN32 "main.cpp")
else()
	add_executable(DynamicMarkupExample "main.cpp")
endif()

klemmui_resources(DynamicMarkupExample "../res/")
klemmui_web_project(DynamicMarkupExample)

target_link_libraries(DynamicMarkupExample PRIVATE KuiDynamicMarkup)
target_link_libraries(DynamicMarkupExample PUBLIC KlemmUI)