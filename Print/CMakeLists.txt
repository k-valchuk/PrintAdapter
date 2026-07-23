cmake_minimum_required(VERSION 3.16)

project(PrintAdapter VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

#add_compile_definitions(LIVEWIN_EXPORTS)

if("$ENV{SVN_REVISION}" STREQUAL "")
    set(SVN_REVISION 1)
else()
    set(SVN_REVISION $ENV{SVN_REVISION})
endif()

add_compile_definitions(VER_REVISION_STR="${SVN_REVISION}")
add_compile_definitions(VER_REVISION=${SVN_REVISION})

find_package(Qt5 COMPONENTS Widgets Core Network PrintSupport REQUIRED)

if(${BUILDING_FROM_SOLUTION})
    set(BRAM_SPACE_INC_DIR "../../../../BramSpace/${BRAM_SPACE_VER}/Client/Include")
	set(DEPS_SLASH "/")
    set(QT_COMPONENTS_SOURCES_SUBDIR "Sources")
else()
	set(BRAM_SPACE_INC_DIR "../Dependencies/BramSpace/Include")
        set(LIB_QTPROPERTYEDITOR_VER "")
	set(DEPS_SLASH "")
endif()

set(QTCOMPONENTS_SUBPATH ${DEPS_SLASH}${QT_COMPONENTS_SOURCES_SUBDIR}${DEPS_SLASH})

message(${BRAM_SPACE_INC_DIR})

include_directories(
                    src/
                    src/PrintAdapter
                    ${BRAM_SPACE_INC_DIR}/
                    ../Dependencies/Query${DEPS_SLASH}/Include/
                    ../Dependencies
                    ../Include/
                    
                    )

set(SOURCES     
        src/PrintModule.cpp
        src/PrintModule.h
        src/PrintModuleExport.h
        src/PrintModuleExport.cpp
                
        #resourse

        res/print_res.qrc

        src/PrintAdapter/AddLine.h
        src/PrintAdapter/AddLine.cpp
        src/PrintAdapter/BaseDelegate.h
        src/PrintAdapter/BaseDialog.h
        src/PrintAdapter/BaseDialog.cpp
        src/PrintAdapter/BaseTableView.h
        src/PrintAdapter/BaseTableView.cpp
        src/PrintAdapter/Config.h
        src/PrintAdapter/EditDialog.h
        src/PrintAdapter/EditDialog.cpp
        src/PrintAdapter/PageSetupDialog.h
        src/PrintAdapter/PageSetupDialog.cpp
        src/PrintAdapter/PrintableReport.h
        src/PrintAdapter/PrintableReport.cpp
        src/PrintAdapter/PrintDialog.h
        src/PrintAdapter/PrintDialog.cpp
        src/PrintAdapter/RemoveConfirmDialog.h
        src/PrintAdapter/RemoveConfirmDialog.cpp
        src/PrintAdapter/RequestDialog.h
        src/PrintAdapter/RequestDialog.cpp
        src/PrintAdapter/Requests.h
        src/PrintAdapter/Requests.cpp
        src/PrintAdapter/RowDelegate.h
        src/PrintAdapter/RowDelegate.cpp
        src/PrintAdapter/ServerRequester.h
        src/PrintAdapter/ServerRequester.cpp
        src/PrintAdapter/ShowRowDelegate.h
        src/PrintAdapter/ShowRowDelegate.cpp
        src/PrintAdapter/TemplateHighlighter.h
        src/PrintAdapter/TemplateHighlighter.cpp
        src/PrintAdapter/TemplateManager.h
        src/PrintAdapter/TemplateManager.cpp
        src/PrintAdapter/SubsystemButton.h
        src/PrintAdapter/SubsystemButton.cpp
        src/PrintAdapter/CheckPanel.h
        src/PrintAdapter/CheckPanel.cpp

        src/version.h

        ../Include/IPrintInterface.h
                

                

)

if(WIN32)
    set(SPECPLATFORM_LIBS )
else()
    set(SPECPLATFORM_LIBS pthread)
endif (WIN32)

if(WIN32)
    set(WINVERSIONRC src/resources.rc)
    set(OS_path "win32")
elseif(UNIX AND NOT APPLE)
    set(OS_path "linux")
endif()
if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    set (PROC_path "x64")
else ()
    set (PROC_path "x86")
endif ()

set(WHOLE_path "${OS_path}-${PROC_path}")

link_directories(../Dependencies/Lib/${WHOLE_path}/
                )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "../../../../bin/${OS_path}-debug-${PROC_path}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "../../../../bin/${OS_path}-debug-${PROC_path}")   # Visual Studio "feature"
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "../../../../bin/${OS_path}-release-${PROC_path}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "../../../../bin/${OS_path}-release-${PROC_path}") # Visual Studio "feature"
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "../../../../bin/${OS_path}-release-${PROC_path}") # Visual Studio "feature"
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "../../../../bin/${OS_path}-release-${PROC_path}") # Visual Studio "feature"
endif()

#установить в true если требуется обновить ts filы по исходникам
#в таком случае Очистка или Пересборка удалит их, будьте ОСТОРОЖНЫ!!!
add_compile_definitions(_USES_LIB_QUERY)
add_library(PrintAdapter SHARED ${SOURCES}
    ${QRC_SRCS_NEWSBROADCASTER} 
    ${QM_FILES_NEWSBROADCASTER} #это только для того чтобы была зависимость и выполнялся update
    ${WINVERSIONRC}
)
set(COMMON_LIBS
    Qt5::Widgets
    Qt5::Network
    Qt5::PrintSupport
    ${CMAKE_DL_LIBS})
target_link_libraries(PrintAdapter lib_Query ${COMMON_LIBS} ${SPECPLATFORM_LIBS})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG_OR_RELEASE "debug")
else()
    set(DEBUG_OR_RELEASE "release")
endif()
set(PROJECT_BINARY_DIR ../../../../bin/${OS_path}-${DEBUG_OR_RELEASE}-${PROC_path})
#set_target_properties(NewsBroadCaster PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set_target_properties(PrintAdapter PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
     
     
     
# Strip debug information
if(UNIX)
set(project_path_name $<TARGET_FILE:${PROJECT_NAME}>)
get_filename_component(project_file_name_wo_prefix ${project_path_name} NAME_WE)

add_custom_command(
        TARGET PrintAdapter POST_BUILD
                # Strip debug information to separate file
        COMMAND objcopy --only-keep-debug ${project_path_name} ${project_file_name_wo_prefix}.dbg
        COMMAND strip -g --strip-unneeded ${project_path_name}
        COMMAND objcopy --add-gnu-debuglink=${project_file_name_wo_prefix}.dbg ${project_path_name}
        VERBATIM)
endif()
