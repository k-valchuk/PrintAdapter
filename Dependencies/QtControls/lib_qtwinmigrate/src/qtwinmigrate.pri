INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
LIBS += -luser32\
        -lGdi32

HEADERS += $$PWD/qwinwidget.h\
           $$PWD/qwinhost.h\
           $$PWD/qmfcapp.h

SOURCES += $$PWD/qwinwidget.cpp\
           $$PWD/qwinhost.cpp\
            $$PWD/qmfcapp.cpp

