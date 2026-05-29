cmake_minimum_required(VERSION 3.16)

project(lib_ViewList VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS 
    ../src/FileExplorer/ui 
    ../src/CustomCalendar/ui
    ../src/CustomComboBox/
    ../src/LoadingOverlay
)


find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

include_directories(../src/ViewList
                    ../src/TCEditWidget
                    ../src/FileExplorer
                    ../src/FocusPropertyFilter
                    ../src/CustomCalendar
                    ../src/AnimatedGroupBox
                    ../src/RowHoverDelegate
                    ../src/AutoBoxLayout
                    ../src/CustomComboBox
                    )
set(SOURCES  ../src/ViewList/treeview.cpp
             ../src/ViewList/treeitem.cpp
             ../src/ViewList/treemodel.cpp
             ../src/ViewList/headerview.cpp
             ../src/ViewList/writablelocation.cpp
             ../src/ViewList/styleItemDelegate.cpp
             ../src/ViewList/dropIndicatorProxyStyle.cpp
             ../src/ViewList/customDragViewList.cpp
             ../src/ViewList/hotKeyEditor.cpp
             ../src/ViewList/ItemComboBox.cpp
             ../src/ViewList/itemTextEdit.cpp
             ../src/ViewList/TreeViewColumnResizer.cpp
             ../src/ViewList/TreeGroupingStyleItemDelegate.h 
             ../src/ViewList/TreeGroupingStyleItemDelegate.cpp
             #OnlyLongerContentToolTipper
             ../src/ViewList/OnlyLongerContentToolTipper.h
             ../src/ViewList/OnlyLongerContentToolTipper.cpp
             
             #ComboBox
             ../src/CheckComboBox/checkCombobox.cpp
             #Logger
             ../src/Logger/Logger.cpp
             #TCEditWidget
             ../src/TCEditWidget/TCEditWidget.cpp
             ../src/TCEditWidget/TCOperations.cpp
             #filexplorer
             ../src/FileExplorer/dirDialogTree.cpp
             ../src/FileExplorer/fileDialogTree.cpp
             ../src/FileExplorer/FileDialogWidget.cpp
             ../src/FileExplorer/FileDialogWidget.ui
             ../src/FileExplorer/thumbnailCreator.cpp
             
             #spoiler
             ../src/Spoiler/Spoiler.h
             ../src/Spoiler/Spoiler.cpp
             ../src/Spoiler/SpoilerToolButton.h
             ../src/Spoiler/SpoilerToolButton.cpp

             #Custom Calendar
             ../src/CustomCalendar/CustomCalendar.h
             ../src/CustomCalendar/CustomCalendar.cpp
             ../src/CustomCalendar/ui/CustomCalendar.ui
             
             ../src/CustomCalendar/CustomDateTimeEdit.h
             ../src/CustomCalendar/CustomDateTimeEdit.cpp
             #../src/CustomCalendar/ui/CustomDateTimeEdit.ui
             
             ../src/CustomCalendar/CustomIntervalEdit.h
             ../src/CustomCalendar/CustomIntervalEdit.cpp
             ../src/CustomCalendar/ui/CustomIntervalEdit.ui
             
             #Loading overlay
             ../src/LoadingOverlay/LoadingOverlay.h
             ../src/LoadingOverlay/LoadingOverlay.cpp
             ../src/LoadingOverlay/LoadingOverlay.ui
             
             #Flow layout
             ../src/FlowLayout/FlowLayout.h
             ../src/FlowLayout/FlowLayout.cpp
             #AutoBoxLayout
             ../src/AutoBoxLayout/AutoBoxLayout.h
             ../src/AutoBoxLayout/AutoBoxLayout.cpp
             
             #Switcher
             ../src/Switcher/Switcher.h 
             ../src/Switcher/Switcher.cpp
             
             ../src/Switcher/OnOffSwitch.h
             ../src/Switcher/OnOffSwitch.cpp
             
             #CustomComboBox
             ../src/CustomComboBox/CustomComboBox.h
             ../src/CustomComboBox/CustomComboBox.cpp
             ../src/CustomComboBox/CustomComboBox.ui
             
             #CustomTextEdit
             ../src/CustomTextEdit/CustomTextEdit.h
             ../src/CustomTextEdit/CustomTextEdit.cpp
             ../src/CustomTextEdit/TextWidgetObjectHandler.h
             ../src/CustomTextEdit/TextWidgetObjectHandler.cpp
             
             #HighlightedScrollBar
             ../src/HighlightedScrollBar/HighlightedScrollBar.h
             ../src/HighlightedScrollBar/HighlightedScrollBar.cpp
)

#set(TS_FILES ../Resources/lang/QtControls_ru.ts)
#qt5_add_translation(QM_FILES ${TS_FILES})

set(COMMON_LIBS Qt5::Widgets ${CMAKE_DL_LIBS})

add_library(lib_QtControls ${SOURCES} 
#    ${QM_FILES}
)

target_link_libraries(lib_QtControls ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_QtControls PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
