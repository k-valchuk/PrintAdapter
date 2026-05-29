cmake_minimum_required(VERSION 3.16)

project(lib_QtFilter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ../src/Filter/ui ../src/QuickFilter/ui)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

include_directories(
                    ../src/Filter
                    ../src/QuickFilter
                    ../src/boolinq
                    ../src/FlowLayout
                    )
set(SOURCES  
    
    ../src/Filter/res/filter.qrc
    
    ../src/Filter/FilterEllipse.h
    ../src/Filter/FilterEllipse.cpp
    ../src/Filter/ui/FilterEllipse.ui
    
    ../src/Filter/IFilterElement.h 
    
    ../src/Filter/TextFilter.h 
    ../src/Filter/TextFilter.cpp
    ../src/Filter/DateFilter.h 
    ../src/Filter/DateFilter.cpp
    ../src/Filter/NumericFilter.h 
    ../src/Filter/NumericFilter.cpp
    ../src/Filter/ComboFilter.h 
    ../src/Filter/ComboFilter.cpp
    
    
    ../src/Filter/FiltersList.h
    ../src/Filter/FiltersList.cpp
    ../src/Filter/ui/FiltersList.ui
    ../src/Filter/FiltersListListView.h
    
    ../src/Filter/OperatorList.h
    ../src/Filter/OperatorList.cpp
    ../src/Filter/ui/OperatorList.ui
    
    ../src/Filter/FilterRow.h
    ../src/Filter/FilterRow.cpp
    ../src/Filter/ui/FilterRow.ui
    
    ../src/Filter/FilterExpressions.h
    ../src/Filter/FilterExpressions.cpp
    ../src/Filter/FilterRowNode.h
    
    
    #QuickFilter
    
    ../src/QuickFilter/QuickFilterItem.h
    ../src/QuickFilter/QuickFilterItem.cpp
    
    ../src/QuickFilter/QuickFilterPanel.h 
    ../src/QuickFilter/QuickFilterPanel.cpp 
    ../src/QuickFilter/ui/QuickFilterPanel.ui
    
    ../src/QuickFilter/QuickFilterTabButtons.h 
    ../src/QuickFilter/QuickFilterTabButtons.cpp 
    ../src/QuickFilter/ui/QuickFilterTabButtons.ui
    
    
    ../src/QuickFilter/QFCategoryBox.h 
    ../src/QuickFilter/QFCategoryBox.cpp 
    ../src/QuickFilter/ui/QFCategoryBox.ui
    ../src/QuickFilter/QFSpoiler.h 
    ../src/QuickFilter/QFSpoiler.cpp 
    
    
    ../src/QuickFilter/CustomIconButtonCorner.cpp
    ../src/QuickFilter/CustomIconButtonCorner.h
    
    #Filter preset
    ../src/QuickFilter/FilterPresetItem.h
    
    ../src/QuickFilter/ReordarableVLayout.h 
    ../src/QuickFilter/ReordarableVLayout.cpp
    ../src/QuickFilter/DraggableFrame.h 
    ../src/QuickFilter/DraggableFrame.cpp
    ../src/QuickFilter/FilterPresetRow.h 
    ../src/QuickFilter/FilterPresetRow.cpp
    ../src/QuickFilter/ui/FilterPresetRow.ui
    ../src/QuickFilter/FilterPresetListWidget.h 
    ../src/QuickFilter/FilterPresetListWidget.cpp
    
    ../src/QuickFilter/FilterHistoryRow.h 
    ../src/QuickFilter/FilterHistoryRow.cpp
    ../src/QuickFilter/ui/FilterHistoryRow.ui
    ../src/QuickFilter/FilterHistoryListWidget.h 
    ../src/QuickFilter/FilterHistoryListWidget.cpp
    
    #FilterPanel (Filter + QuickFilter template panel)
    ../src/FilterPanel/FilterPanel.cpp
    ../src/FilterPanel/FilterPanel.h
    ../src/FilterPanel/FilterPanel.ui
    
    
    ../src/QuickFilter/ElidedLabel.h 
    ../src/QuickFilter/ElidedLabel.cpp
    
    ../src/QuickFilter/FilterRowToolTip.h 
    ../src/QuickFilter/FilterRowToolTip.cpp
)


set(COMMON_LIBS Qt5::Widgets ${CMAKE_DL_LIBS})

add_library(lib_QtFilter ${SOURCES} 
    
)

target_link_libraries(lib_QtFilter lib_QtControls ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_QtFilter PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
