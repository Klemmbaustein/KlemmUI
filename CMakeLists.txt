cmake_minimum_required(VERSION 3.15)

project(KlemmUI C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

set(SRC_DIR "Source")
set(INC_DIR "Include")

file(
	GLOB_RECURSE
	SRCS
	"${SRC_DIR}/**.cpp"
)
file(
	GLOB_RECURSE
	INCLUDES
	"${INC_DIR}/**.h"
)

option(KLEMMUI_WEB "Build the library with emscripten" OFF)
option(KLEMMUI_WEB_RESOURCE_PATH "Resource and UI headers path for a web build" "")
option(KLEMMUI_WEB_BUILD_AS_HTML "Build web projects as HTML" ON)
option(KLEMMUI_DYNAMIC_MARKUP "Use DynamicMarkup library to load .kui files at rumtime" OFF)
option(KLEMMUI_INCLUDE_EXAMPLES "Include example programs" ON)
option(KLEMMUI_CUSTOM_SYSTEMWM "Implement custom systemWM functions" OFF)

set(glew-cmake_BUILD_SHARED OFF)
set(ONLY_LIBS ON)
if(NOT KLEMMUI_WEB)
	add_subdirectory("Dependencies/glew-cmake")
endif()

add_library(
	KlemmUI STATIC
	${SRCS} ${INCLUDES}
)

if(KLEMMUI_CUSTOM_SYSTEMWM)
	message(STATUS "Not including predefined kui::systemWM::* functions for platform.")
	target_compile_definitions(KlemmUI PRIVATE KLEMMUI_CUSTOM_SYSTEMWM)
endif()

if(KLEMMUI_WEB)
	message("Building for the web...")
	# m? i wish the linux people gave their things names that remotely make sense...
	target_link_libraries(KlemmUI PUBLIC GLESv2 EGL m X11)
	target_compile_definitions(KlemmUI PUBLIC KLEMMUI_WEB_BUILD=1)
elseif(NOT DEFINED WIN32)
	option(KLEMMUI_USE_XRANDR "Use the Xrandr X11 extension if available" ON)
	option(KLEMMUI_WITH_WAYLAND "Include wayland support" ON)

	message(STATUS "Checking for X11 extensions")
		
	if(KLEMMUI_WITH_WAYLAND)

		find_library(KLEMMUI_WAYLAND_DBUS dbus-1)
		find_package(PkgConfig) 
		pkg_check_modules(KLEMMUI_DBUS IMPORTED_TARGET GLOBAL dbus-1)

		target_link_libraries(KlemmUI PRIVATE decor-0 wayland-client wayland-cursor wayland-egl EGL xkbcommon)
		target_compile_definitions(KlemmUI PUBLIC KLEMMUI_WITH_WAYLAND)

		if(PkgConfig_FOUND AND KLEMMUI_DBUS_FOUND)
			message(STATUS "With dbus")
			target_link_libraries(KlemmUI PRIVATE PkgConfig::KLEMMUI_DBUS)
			target_compile_definitions(KlemmUI PRIVATE HAS_DBUS=1)
		else()
			message(STATUS "Without dbus, PkgConfig or libdbus-1 could not be found")
		endif()

		target_sources(KlemmUI PRIVATE "Source/SystemWM/libdecor/desktop-settings.c")
	endif()

	find_library(KLEMMUI_XRANDR_LIB Xrandr)
	if(KLEMMUI_USE_XRANDR AND KLEMMUI_XRANDR_LIB)
		target_compile_definitions(KlemmUI PRIVATE KLEMMUI_USE_XRANDR)
		target_link_libraries(KlemmUI PUBLIC ${KLEMMUI_XRANDR_LIB})
		message(STATUS "Using Xrandr X11 extension")
	endif()

	# GCC defines 'linux' as a macro by default (what?), which conflicts with 'kui::platform::linux'
	target_compile_options(KlemmUI PUBLIC "-Ulinux" "-Wno-psabi")
endif()

add_subdirectory("Util/Markup/")
add_subdirectory("Util/Resource/")

target_include_directories(KlemmUI PUBLIC "${INC_DIR}")

if(NOT KLEMMUI_WEB)
	target_link_libraries(KlemmUI PUBLIC libglew_static)
endif()

macro(klemmui_markup ProjectName MarkupDir)
	set(OutDir "${CMAKE_CURRENT_BINARY_DIR}/ui")

	file(
		GLOB_RECURSE
		UI_FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/${MarkupDir}/**"
	)

	set(UI_HEADERS)
	foreach(UI_FILE ${UI_FILES})
		get_filename_component(HEADER_FILE ${UI_FILE} NAME_WE)
		set(HEADER_FILE "${OutDir}/${HEADER_FILE}.kui.hpp")
		list(APPEND UI_HEADERS ${HEADER_FILE})
	endforeach()

	file(MAKE_DIRECTORY ${OutDir})

	if(UI_HEADERS AND NOT KLEMMUI_WEB)
		target_sources(${ProjectName} PRIVATE ${UI_FILES})

		add_custom_command(
			OUTPUT ${UI_HEADERS}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMAND $<TARGET_FILE:KlemmUIHT>  -i ${MarkupDir} -o ${OutDir}
			DEPENDS ${UI_FILES}
		)

		add_custom_target(${ProjectName}_Headers DEPENDS ${UI_HEADERS})

		add_dependencies(${ProjectName}_Headers KlemmUIHT)
		add_dependencies(${ProjectName} ${ProjectName}_Headers)
		target_include_directories(${ProjectName} PUBLIC ${OutDir})
	else()
		target_include_directories(${ProjectName} PUBLIC ${CMAKE_SOURCE_DIR}/${KLEMMUI_WEB_RESOURCE_PATH}/headers)
	endif()
endmacro()

macro(klemmui_resources ProjectName ResourceDir)

	set(RESOURCE_OUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/res/${ProjectName}_res.c)

	file(
		GLOB_RECURSE
		RESOURCES
		"${ResourceDir}/**"
	)

	if(NOT KLEMMUI_WEB)
		add_custom_command(
			OUTPUT ${RESOURCE_OUT_FILE}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMAND $<TARGET_FILE:KlemmUIRC>  -i "${ResourceDir}" -o "${RESOURCE_OUT_FILE}" -n ${ProjectName}
			DEPENDS ${RESOURCES}
		)
	else()
		set(RESOURCE_OUT_FILE ${CMAKE_SOURCE_DIR}/${KLEMMUI_WEB_RESOURCE_PATH}/${ProjectName}_res.c)
	endif()

	if(NOT KLEMMUI_WEB OR EXISTS ${RESOURCE_OUT_FILE})
		add_library(${ProjectName}_Resources STATIC ${RESOURCE_OUT_FILE})

		add_dependencies(${ProjectName}_Resources KlemmUIRC)
		target_link_libraries(KlemmUI PUBLIC ${ProjectName}_Resources)

		if(NOT ${ProjectName} STREQUAL "KlemmUI")
			target_compile_definitions(KlemmUI PUBLIC KLEMMUI_USE_RESOURCES)
		endif()
		message(STATUS "Adding resource dir - ${ResourceDir}")
	endif()
endmacro()

# Makes executables compile as a web project.
macro(klemmui_web_project TargetName)
	if(KLEMMUI_WEB)
		set_target_properties(${TargetName} PROPERTIES
			LINK_FLAGS "-s USE_GLFW=3 -s ASYNCIFY -s MIN_WEBGL_VERSION=2 --bind -gsource-map --emrun")
		if(KLEMMUI_WEB_BUILD_AS_HTML)
			set_target_properties(${TargetName}
				PROPERTIES SUFFIX ".html")
		endif()
	endif()
endmacro()


klemmui_resources(KlemmUI "Resources/")

if(KLEMMUI_DYNAMIC_MARKUP)
	add_subdirectory("Util/DynamicMarkup")
endif()

if(KLEMMUI_INCLUDE_EXAMPLES AND ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	if(KLEMMUI_DYNAMIC_MARKUP)
		add_subdirectory("Examples/DynamicMarkup")
	endif()
	add_subdirectory("Examples/HelloWorld")
	add_subdirectory("Examples/Translation")
endif()
