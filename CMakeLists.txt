###############################################################
######                                                   ######
######			Author: Pablo R.S. (aka. Bardo91)        ######
######                                                   ######
###############################################################

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project(multi_client_server)
set(PROJECT_VERSION 0.2.1)

# Clasify sources according to folder structure.
# This creates a nice layout for the project in visual studio
# This macro is courtesy of http://www.cmake.org/pipermail/cmake/2013-November/056336.html
macro(GroupSources curdir)
	file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
		${PROJECT_SOURCE_DIR}/${curdir}/*)
	foreach(child ${children})
		if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			GroupSources(${curdir}/${child})
		else()
			string(REPLACE "/" "\\" groupname ${curdir})
			source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
		endif()
	endforeach()
endmacro()

GroupSources(src)

#########################################
###### Loading 3rd party libraries.######
#########################################
 
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})

find_package(catkin COMPONENTS roscpp)

if(${catkin_FOUND})
	catkin_package(
		INCLUDE_DIRS ./include
	)
endif()

## Loading boost
if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost REQUIRED COMPONENTS system date_time regex)

#########################################
######			Executable		   ######
#########################################
IF(UNIX)
	add_definitions(-std=c++11)
ENDIF()


file(GLOB_RECURSE LIB_SRC_FILES "src/*.cpp")
file(GLOB_RECURSE LIB_HEADER_FILES "include/*.h" "include/*.inl" )

GroupSources(src)
GroupSources(include)
add_library(${PROJECT_NAME} ${LIB_SRC_FILES} ${LIB_HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${catkin_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${catkin_LIBRARIES})

GroupSources(examples)

macro(MCS_APP app_name main_file)
	add_executable(${app_name} ${main_file} ${LIB_HEADER_FILES})
	target_include_directories(${app_name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(${app_name} LINK_PUBLIC ${PROJECT_NAME})
	
	target_include_directories(${PROJECT_NAME} PUBLIC ${boost_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${boost_LIBRARIES})

	target_include_directories(${app_name} PUBLIC ${catkin_INCLUDE_DIRS})
	target_link_libraries(${app_name} LINK_PUBLIC ${catkin_LIBRARIES})
endmacro()

MCS_APP(example_tcp_server examples/main_tcp_server.cpp)
MCS_APP(example_tcp_client examples/main_tcp_client.cpp)
MCS_APP(example_udp_server examples/main_udp_server.cpp)
MCS_APP(example_udp_client examples/main_udp_client.cpp)
