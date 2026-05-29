cmake_minimum_required(VERSION 3.16)

project(lib_ConfigNetwork VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

if(${BUILDING_FROM_SOLUTION})
        set(DEPS_DIR "../../../../CommonComponents/ClientComponents")
        set(DEPS_SLASH "/")
        set(LIB_QUERY_VER "1.0")
else()
        set(DEPS_DIR "..")
        set(DEPS_SLASH "/")
        set(LIB_QUERY_VER "")
endif()

include_directories(
                        ./
                        Client/
                        ${DEPS_DIR}/Query${DEPS_SLASH}${LIB_QUERY_VER}/Include
                        ${DEPS_DIR}/QtWSNotifier${DEPS_SLASH}${LIB_NOTIFIER_VER}/include
                        #../Dependencies/QtWSNotifier/include/
                        ${DEPS_DIR}/Network${DEPS_SLASH}${LIB_NETWORK_VER}/Notifier/
                        ${DEPS_DIR}/Network${DEPS_SLASH}${LIB_NETWORK_VER}/Notifier/Include
                    )

set(SOURCES
                Client/ConfigSubscriberNotifier.h
                Client/ConfigSubscriber.h
                Client/ConfigSubscriber.cpp
				
                Client/ConfigStructs.h
                Client/ConfigStructs.cpp
				
                Client/ConfigCommonRequests.h
                Client/ConfigCommonRequests.cpp
                Client/ServerRequests.h
                Client/ServerRequests.cpp
                Client/ServiceRequests.h
                Client/ServiceRequests.cpp
                Client/RightRequests.h
                Client/RightRequests.cpp
)

set(COMMON_LIBS Qt5::Core Qt5::Network ${CMAKE_DL_LIBS})

add_library(lib_ConfigNetwork ${SOURCES})

target_link_libraries(lib_ConfigNetwork lib_Query lib_Notifier ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_ConfigNetwork PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
