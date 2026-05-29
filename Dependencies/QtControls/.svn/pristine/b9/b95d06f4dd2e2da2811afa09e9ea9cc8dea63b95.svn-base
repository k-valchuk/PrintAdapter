cmake_minimum_required(VERSION 3.16)

project(lib_QtVideoPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)

#add_compile_definitions(LIVEWIN_EXPORTS)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(../src/VideoPlayerWidget/Inc/
                    ../src/VideoPlayerWidget/Inc/VInc/
                    )

set(SOURCES     ../src/VideoPlayerWidget/IDataStream.h
		../src/VideoPlayerWidget/IDataStream.cpp
                
                ../src/VideoPlayerWidget/LiveWindowStream.h
                ../src/VideoPlayerWidget/LiveWindowStream.cpp
                
                ../src/VideoPlayerWidget/VideoPlayer.h 
                ../src/VideoPlayerWidget/VideoPlayer.cpp
                
                ../src/VideoPlayerWidget/PeakMeterWidget.h 
                ../src/VideoPlayerWidget/PeakMeterWidget.cpp

                # Прозрачная плашка текста с анимацией opacity
                ../src/VideoPlayerWidget/AnimatedTextLabel.h
                ../src/VideoPlayerWidget/AnimatedTextLabel.cpp

                # Максимальное значение пика (для отсечки пикметра)
                ../src/VideoPlayerWidget/MaxPeakValue.h
                ../src/VideoPlayerWidget/MaxPeakValue.cpp
)

set(COMMON_LIBS Qt5::Widgets ${CMAKE_DL_LIBS})

add_library(lib_QtVideoPlayer ${SOURCES})

target_link_libraries(lib_QtVideoPlayer LiveWindow ${COMMON_LIBS})

set(PROJECT_BINARY_DIR ../../Lib64)
set_target_properties(lib_QtVideoPlayer PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
