TEMPLATE        = lib
TARGET          = _QtCommon

win32{
DESTDIR         = ../../Lib32
}
unix{
DESTDIR         = ../../Lib64
}

CONFIG	      	+= staticlib qt warn_on
QT		+= widgets sql

MOC_DIR         = tmp/moc
OBJECTS_DIR     = tmp/obj
RCC_DIR         = tmp/rcc
UI_DIR          = tmp/ui

!macx {
QMAKE_CXXFLAGS += -std=c++11 -g
}

CONFIG(release, debug|release): CONFIG += force_debug_info

include(lib_QtCommon.pri)
