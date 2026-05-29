cmake_minimum_required(VERSION 3.16)

project(lib_QtSharedDataManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ../src/SharedDataManager)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

include_directories(
                    ../src/SharedDataManager
                    ../src/ViewList/
                    )
set(SOURCES  
    
    ../src/SharedDataManager/ISharedDataModel.h
            
    ../src/SharedDataManager/SharedDataVM.h
    ../src/SharedDataManager/SharedDataVM.cpp
    ../src/SharedDataManager/SharedDataTableModel.h
    
    ../src/SharedDataManager/IndividualUserDataForm.h
    ../src/SharedDataManager/IndividualUserDataForm.cpp
    ../src/SharedDataManager/IndividualUserDataForm.ui
    
    ../src/SharedDataManager/GroupDataForm.h 
    ../src/SharedDataManager/GroupDataForm.cpp 
    ../src/SharedDataManager/GroupDataForm.ui
    
    ../src/SharedDataManager/UsersGroupsSelectionForm.h 
    ../src/SharedDataManager/UsersGroupsSelectionForm.cpp 
    ../src/SharedDataManager/UsersGroupsSelectionForm.ui
)


set(COMMON_LIBS Qt5::Widgets ${CMAKE_DL_LIBS})

add_library(lib_QtSharedDataManager ${SOURCES} 
    
)

target_link_libraries(lib_QtSharedDataManager lib_QtControls ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_QtSharedDataManager PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
