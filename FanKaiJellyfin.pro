QT += quick network

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        jellyfin.cpp \
        main.cpp \
        media/audiostream.cpp \
        media/mediasource.cpp \
        media/mediastream.cpp \
        media/subtitlestream.cpp \
        media/videostream.cpp \
        nodes/episode.cpp \
        nodes/indexednode.cpp \
        nodes/node.cpp \
        nodes/season.cpp \
        nodes/series.cpp \
        qmllinker.cpp \
        user.cpp

RESOURCES += qml.qrc \
    images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    jellyfin.h \
    media/audiostream.h \
    media/mediasource.h \
    media/mediastream.h \
    media/subtitlestream.h \
    media/videostream.h \
    nodes/episode.h \
    nodes/indexednode.h \
    nodes/node.h \
    nodes/season.h \
    nodes/series.h \
    qmllinker.h \
    user.h

INCLUDEPATH += -L$$PWD/VLCQt/include

LIBS += -L$$PWD/VLC/lib
LIBS += -L$$PWD/VLCQt/lib

LIBS += -llibvlc
LIBS += -llibvlccore

win32:CONFIG(release, debug|release) {
    LIBS += -llibVLCQtCore.dll
    LIBS += -llibVLCQtQml.dll
    LIBS += -llibVLCQtWidgets.dll
} else:win32:CONFIG(debug, debug|release) {
    LIBS += -llibVLCQtCored.dll
    LIBS += -llibVLCQtQmld.dll
    LIBS += -llibVLCQtWidgetsd.dll
}

RC_ICONS = logo.ico
