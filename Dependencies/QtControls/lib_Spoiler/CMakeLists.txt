cmake_minimum_required(VERSION 3.16)

project(lib_Spoiler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

#set(STANDALONE_BUILD true)

if(${STANDALONE_BUILD})
        set(SOURCE_DIR "../src")
else()
        set(SOURCE_DIR "src")
endif()

include_directories(${SOURCE_DIR}/Spoiler/)

set(SOURCES 
        ${SOURCE_DIR}/Spoiler/Spoiler.h
        ${SOURCE_DIR}/Spoiler/Spoiler.cpp
        ${SOURCE_DIR}/Spoiler/SpoilerToolButton.h
        ${SOURCE_DIR}/Spoiler/SpoilerToolButton.cpp
)

set(COMMON_LIBS Qt5::Widgets ${CMAKE_DL_LIBS})

add_library(lib_Spoiler ${SOURCES})

target_link_libraries(lib_Spoiler ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_Spoiler PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
