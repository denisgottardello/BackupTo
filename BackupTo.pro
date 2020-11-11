#-------------------------------------------------
#
# Project created by QtCreator 2018-08-16T01:07:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BackupTo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix:!mac {
  LIBS += -Wl,-rpath=\\\$$ORIGIN/libs
}

SOURCES += \
        main.cpp \
        qfmainwindow.cpp \
    commons.cpp \
    qdtextinput.cpp \
    qthsynchronize.cpp \
    qdfileattributes.cpp \
    qdemptydirectory.cpp \
    qthemptydirectory.cpp

HEADERS += \
        qfmainwindow.h \
    commons.h \
    qdtextinput.h \
    qthsynchronize.h \
    qdfileattributes.h \
    qdemptydirectory.h \
    qthemptydirectory.h

FORMS += \
        qfmainwindow.ui \
    qdtextinput.ui \
    qdfileattributes.ui \
    qdemptydirectory.ui

TRANSLATIONS = BackupTo_it.ts

RC_FILE = BackupTo.rc

OTHER_FILES += \
    qt.conf

ICON = BackupTo.icns

DISTFILES += \
    BackupTo_it.ts

RESOURCES += \
    ResourceFile.qrc
