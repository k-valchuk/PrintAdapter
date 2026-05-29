cmake_minimum_required(VERSION 3.16)

project(lib_Query VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#add_compile_definitions(LIB_QUERY_DLLEXPORT)

if("$ENV{SVN_REVISION}" STREQUAL "")
    set(SVN_REVISION 1)
else()
    set(SVN_REVISION $ENV{SVN_REVISION})
endif()

add_compile_definitions(VER_REVISION_STR="${SVN_REVISION}")
add_compile_definitions(VER_REVISION=${SVN_REVISION})

find_package(Qt5 COMPONENTS Core Network LinguistTools REQUIRED)

include_directories(
    Include/
    src/
                    )

set(SOURCES   
    Include/Query.h
    Include/QueryExecutor.h 
    Include/Response.h 
    Include/Request.h
	
    src/Query.cpp
    src/QueryExecutor.cpp
    src/Response.cpp
    src/Request.cpp
)

set(COMMON_LIBS Qt5::Core Qt5::Network ${CMAKE_DL_LIBS})

#if(WIN32)
#    set(WINVERSIONRC src/resources.rc)
#    set(OS_path "win32")
#elseif(UNIX AND NOT APPLE)
#    set(OS_path "linux")
#endif()
#if (CMAKE_SIZEOF_VOID_P MATCHES "8")
#    set (PROC_path "x64")
#else ()
#    set (PROC_path "x86")
#endif ()
#
#set(WHOLE_path "${OS_path}-${PROC_path}")
#
#
#if (CMAKE_BUILD_TYPE MATCHES Debug)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "../../../../bin/${OS_path}-debug-${PROC_path}")
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "../../../../bin/${OS_path}-debug-${PROC_path}")   # Visual Studio "feature"
#else()
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "../../../../bin/${OS_path}-release-${PROC_path}")
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "../../../../bin/${OS_path}-release-${PROC_path}") # Visual Studio "feature"
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../../../../bin/${OS_path}-release-${PROC_path}") # Visual Studio "feature"
#	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "../../../../bin/${OS_path}-release-${PROC_path}") # Visual Studio "feature"
#endif()


add_library(lib_Query 
	${SOURCES}
)

target_link_libraries(lib_Query ${COMMON_LIBS})

#add_library(Query SHARED
#    src/QueryExternal.h
#    src/QueryExternal.cpp
#    src/version.h
#    ${WINVERSIONRC}
#)
#
#target_link_libraries(Query Qt5::Core lib_Query ${CMAKE_DL_LIBS})
#
#
#
#if (CMAKE_BUILD_TYPE MATCHES Debug)
#    set(DEBUG_OR_RELEASE "debug")
#else()
#    set(DEBUG_OR_RELEASE "release")
#endif()
#set(PROJECT_BINARY_DIR ../../../../bin/${OS_path}-${DEBUG_OR_RELEASE}-${PROC_path})
set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_Query PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
#
#
## Strip debug information
#if(UNIX)
#set(project_path_name $<TARGET_FILE:Query>)
#get_filename_component(project_file_name_wo_prefix ${project_path_name} NAME_WE)
#
#add_custom_command(
#        TARGET Query POST_BUILD
#		# Strip debug information to separate file
#        COMMAND objcopy --only-keep-debug ${project_path_name} ${project_file_name_wo_prefix}.dbg
#        COMMAND strip -g --strip-unneeded ${project_path_name}
#        COMMAND objcopy --add-gnu-debuglink=${project_file_name_wo_prefix}.dbg ${project_path_name}
#        VERBATIM)
#endif()
#
