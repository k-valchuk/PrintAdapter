#-------------------------------------------------
#
# Настройка версии и пути исполняемого файла
#
#-------------------------------------------------

contains(QT_ARCH, i386) {
    ARCH = 32
    POST_CONF = x86
} else {
    ARCH = 64
    POST_CONF = x64
}

win32{
    PREF_CONF = win32
    contains(QT_ARCH, i386): LibPath = win32-x86
    else: LibPath = win32-x64
} else {
    PREF_CONF = linux
    LibPath = linux-x64
}
CONFIG(release, debug|release): CONF = -release-
else:CONFIG(debug, debug|release): CONF = -debug-

DESTDIR         = ../bin/$$PREF_CONF$$CONF$$POST_CONF
MOC_DIR         = build/$$PREF_CONF$$CONF$$POST_CONF/moc
OBJECTS_DIR     = build/$$PREF_CONF$$CONF$$POST_CONF/obj
RCC_DIR         = build/$$PREF_CONF$$CONF$$POST_CONF/rcc
UI_DIR          = build/$$PREF_CONF$$CONF$$POST_CONF/ui

# если ревизия не определена
if(isEmpty(SVN_REVISION)){
    #Версия из SVN
    SVN_REVISION = $$system("svnversion -n ")
    SVN_REVISION = $$replace(SVN_REVISION,"M","")
    SVN_REVISION = $$replace(SVN_REVISION,"S","")
    SVN_REVISION = $$replace(SVN_REVISION,"P","")
    SVNLAST = $$section(SVN_REVISION, :, 1, 1)
    if(!isEmpty(SVNLAST)){
        SVN_REVISION = $$SVNLAST
    }
}
if(isEmpty(PROJECT_VERSION)){
PROJECT_VERSION = 1.0.0
}

VERSION = $${PROJECT_VERSION}.$$SVN_REVISION
DEFINES +=APP_VERSION=\\\"$$VERSION\\\"
