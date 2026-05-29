cmake_minimum_required(VERSION 3.16)

project(lib_Notifier VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Network WebSockets REQUIRED)

include_directories(
    include/
    src/
                    )

set(SOURCES   
    include/Notifier.h
    include/SubscribersNotifier.h

    src/Notifier.cpp
    src/WSConnectionImpl.cpp
    src/WSConnectionImpl.h

    
    src/SubscribersNotifier.cpp
)

set(COMMON_LIBS Qt5::Core Qt5::Network Qt5::WebSockets ${CMAKE_DL_LIBS})

add_library(lib_Notifier ${SOURCES})

target_link_libraries(lib_Notifier ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_Notifier PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
