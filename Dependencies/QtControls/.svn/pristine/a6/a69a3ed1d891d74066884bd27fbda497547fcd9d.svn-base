cmake_minimum_required(VERSION 3.16)

project(lib_QtAdvancedDocking VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)

add_compile_definitions(ADS_STATIC)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(../src/AdvancedDockingSystem
                    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
                    )
set(SOURCES
    ../src/AdvancedDockingSystem/ads_globals.cpp
    ../src/AdvancedDockingSystem/DockAreaWidget.cpp
    ../src/AdvancedDockingSystem/DockAreaTabBar.cpp
    ../src/AdvancedDockingSystem/DockContainerWidget.cpp
    ../src/AdvancedDockingSystem/DockManager.cpp
    ../src/AdvancedDockingSystem/DockWidget.cpp
    ../src/AdvancedDockingSystem/DockingStateReader.cpp
    ../src/AdvancedDockingSystem/DockWidgetTab.cpp
    ../src/AdvancedDockingSystem/FloatingDockContainer.cpp
    ../src/AdvancedDockingSystem/FloatingDragPreview.cpp
    ../src/AdvancedDockingSystem/DockOverlay.cpp
    ../src/AdvancedDockingSystem/DockSplitter.cpp
    ../src/AdvancedDockingSystem/DockAreaTitleBar.cpp
    ../src/AdvancedDockingSystem/ElidingLabel.cpp
    ../src/AdvancedDockingSystem/IconProvider.cpp
    ../src/AdvancedDockingSystem/DockComponentsFactory.cpp
    ../src/AdvancedDockingSystem/DockFocusController.cpp
    
    ../src/AdvancedDockingSystem/AutoHideDockContainer.cpp
    ../src/AdvancedDockingSystem/AutoHideDockContainer.h
    ../src/AdvancedDockingSystem/AutoHideSideBar.cpp
    ../src/AdvancedDockingSystem/AutoHideSideBar.h
    ../src/AdvancedDockingSystem/AutoHideTab.cpp
    ../src/AdvancedDockingSystem/AutoHideTab.h
    ../src/AdvancedDockingSystem/PushButton.cpp
    ../src/AdvancedDockingSystem/PushButton.h
    ../src/AdvancedDockingSystem/ResizeHandle.cpp
    ../src/AdvancedDockingSystem/ResizeHandle.h
    

    #AutohideDockWidget
    #headers
    ../src/AutoHideDockWidget/AutoHideDockWidget.h
    ../src/AutoHideDockWidget/AutoHideDockWidgetTitle.h
    ../src/AutoHideDockWidget/DockWidgetTabBar.h
    ../src/AutoHideDockWidget/DockWidgetTabButton.h
    ../src/AutoHideDockWidget/MainWindowWithAutoHideDocks.h
    #sources
    ../src/AutoHideDockWidget/AutoHideDockWidget.cpp
    ../src/AutoHideDockWidget/AutoHideDockWidgetTitle.cpp
    ../src/AutoHideDockWidget/DockWidgetTabBar.cpp
    ../src/AutoHideDockWidget/DockWidgetTabButton.cpp
    ../src/AutoHideDockWidget/MainWindowWithAutoHideDocks.cpp
)

set(COMMON_LIBS Qt5::Widgets ${CMAKE_DL_LIBS})



if(WIN32)
    add_library(lib_QtAdvancedDocking ${SOURCES})
    target_link_libraries(lib_QtAdvancedDocking ${COMMON_LIBS})
else()
    add_library(lib_QtAdvancedDocking ${SOURCES} ../src/AdvancedDockingSystem/linux/FloatingWidgetTitleBar.cpp)
    target_link_libraries(lib_QtAdvancedDocking ${COMMON_LIBS} xcb)
endif(WIN32)


set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_QtAdvancedDocking PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
