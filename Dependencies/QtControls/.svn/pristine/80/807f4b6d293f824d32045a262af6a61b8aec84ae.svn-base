
TEMPLATE = lib
TARGET = QtAdvancedDocking

win32{

contains(QT_ARCH, i386): LibPath = win32-x86
else: LibPath = win32-x64

}else{
    LibPath = linux-x64
}
DESTDIR = $$PWD/../../Lib/$$LibPath

CONFIG += c++17 debug_and_release adsBuildStatic
DEFINES += QT_DEPRECATED_WARNINGS

QT += core gui widgets

!adsBuildStatic {
	CONFIG += shared
    DEFINES += ADS_SHARED_EXPORT
}
adsBuildStatic {
	CONFIG += staticlib
    DEFINES += ADS_STATIC
}

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
                QMAKE_CXXFLAGS += /utf-8
        }
}

include (lib_QtAdvancedDocking.pri)

MOC_DIR         = build/$$LibPath/moc
OBJECTS_DIR     = build/$$LibPath/obj
RCC_DIR         = build/$$LibPath/rcc
UI_DIR          = build/$$LibPath/ui

CONFIG(release, debug|release): CONFIG += force_debug_info
