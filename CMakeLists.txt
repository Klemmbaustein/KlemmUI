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

set(glew-cmake_BUILD_SHARED OFF)
set(ONLY_LIBS ON)
add_subdirectory("Dependencies/glew-cmake")
add_library(
	KlemmUI STATIC
	${SRCS}
)

add_subdirectory("Util/Markup/")
add_subdirectory("Util/Resource/")

target_include_directories(KlemmUI PUBLIC "${INC_DIR}")
target_link_libraries(KlemmUI PUBLIC libglew_static)

macro(klemmui_markup ProjectName MarkupDir OutDir)
	
	add_custom_target(
		${ProjectName}_WriteHeaders
		COMMENT "Running build command."
	)

	add_custom_command(
		TARGET ${ProjectName}_WriteHeaders
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND $<TARGET_FILE:KlemmUIHT>  -i ${MarkupDir} -o ${OutDir}
	)

	add_dependencies(${ProjectName}_WriteHeaders KlemmUIHT)
	add_dependencies(${ProjectName} ${ProjectName}_WriteHeaders)

	target_include_directories(${ProjectName} PRIVATE ${OutDir})
endmacro()

function(klemmui_resources ProjectName ResourceDir)
	add_custom_target(
		${ProjectName}_WriteResources
		COMMENT "Running build command."
	)

	set(RESOURCE_OUT_FILE ${CMAKE_BINARY_DIR}/res/${ProjectName}_res.c)

	add_custom_command(
		TARGET ${ProjectName}_WriteResources
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND $<TARGET_FILE:KlemmUIRC>  -i ${CMAKE_CURRENT_LIST_DIR}/${ResourceDir} -o ${RESOURCE_OUT_FILE} -n ${ProjectName}
	)

	if(NOT EXISTS ${RESOURCE_OUT_FILE})
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/res)
		file(TOUCH ${RESOURCE_OUT_FILE})
	endif()

	add_library(${ProjectName}_Resources STATIC ${RESOURCE_OUT_FILE})

	add_dependencies(${ProjectName}_WriteResources KlemmUIRC)
	add_dependencies(${ProjectName}_Resources ${ProjectName}_WriteResources)
	target_link_libraries(KlemmUI PUBLIC ${ProjectName}_Resources)
	if(NOT ${ProjectName} STREQUAL "KlemmUI")
		message(STATUS "Using resouces in project")
		target_compile_definitions(KlemmUI PUBLIC KLEMMUI_USE_RESOURCES)
		target_compile_definitions(KlemmUI PUBLIC KLEMMUI_RESOURCE_NAME=${ProjectName})
	endif()
	message(STATUS "Adding resource dir - ${CMAKE_CURRENT_LIST_DIR}/${ResourceDir} -> ${RESOURCE_OUT_FILE}")
endfunction()

klemmui_resources(KlemmUI "Resources/")

add_subdirectory("Examples/HelloWorld" EXCLUDE_FROM_ALL)
