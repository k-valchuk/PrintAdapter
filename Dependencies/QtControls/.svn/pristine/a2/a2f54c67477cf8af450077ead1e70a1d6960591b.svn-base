TEMPLATE        = lib
TARGET          = _QtControls

win32{

contains(QT_ARCH, i386): LibPath = win32-x86
else: LibPath = win32-x64

}else{
    LibPath = linux-x64
}

DESTDIR = $$PWD/../../Lib/$$LibPath

CONFIG	      	+= staticlib qt warn_on c++14
QT		+= widgets sql network widgets-private

MOC_DIR         = build/$$LibPath/moc
OBJECTS_DIR     = build/$$LibPath/obj
RCC_DIR         = build/$$LibPath/rcc
UI_DIR          = build/$$LibPath/ui

include (lib_QtControls.pri)

CONFIG(release, debug|release): CONFIG += force_debug_info
