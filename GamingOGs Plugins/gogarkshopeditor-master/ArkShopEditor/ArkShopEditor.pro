#-------------------------------------------------
#
# Project created by QtCreator 2020-06-24T08:36:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArkShopEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -lws2_32
LIBS += -lcrypt32
LIBS += -LC:/OpenSSL-Win32/lib -llibssl
LIBS += -LC:/OpenSSL-Win32/lib -llibcrypto
#LIBS += -LC:/OpenSSL-Win32/lib -lucrt

QMAKE_LFLAGS += -static -lpthread

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        addkitwidget.cpp \
        createshopitemwidget.cpp \
        dinowidget.cpp \
        editkitwidget.cpp \
        editshopbeaconwidget.cpp \
        editshopcommandwidget.cpp \
        editshopdinowidget.cpp \
        editshopengramwidget.cpp \
        editshopexperiencewidget.cpp \
        editshopitemwidget.cpp \
        itemwidget.cpp \
        kitswidget.cpp \
        main.cpp \
        mainwindow.cpp \
        sellstoreitemwidget.cpp \
        sellstorewidget.cpp \
        shopstorewidget.cpp

HEADERS += \
        addkitwidget.h \
        createshopitemwidget.h \
        dinowidget.h \
        editkitwidget.h \
        editshopbeaconwidget.h \
        editshopcommandwidget.h \
        editshopdinowidget.h \
        editshopengramwidget.h \
        editshopexperiencewidget.h \
        editshopitemwidget.h \
        internal/dino.h \
        internal/fifo_map.hpp \
        internal/item.h \
        internal/json.hpp \
        internal/json_utils.h \
        internal/kit.h \
        internal/kits.h \
        internal/node.h \
        internal/sellstore.h \
        internal/sellstoreitem.h \
        internal/shopbeacon.h \
        internal/shopcommand.h \
        internal/shopdino.h \
        internal/shopengram.h \
        internal/shopexperience.h \
        internal/shopitem.h \
        internal/shopstore.h \
        itemwidget.h \
        kitswidget.h \
        mainwindow.h \
        sellstoreitemwidget.h \
        sellstorewidget.h \
        shopstorewidget.h \
        httplib.h

FORMS += \
        addkitwidget.ui \
        createshopitemwidget.ui \
        dinowidget.ui \
        editkitwidget.ui \
        editshopbeaconwidget.ui \
        editshopcommandwidget.ui \
        editshopdinowidget.ui \
        editshopengramwidget.ui \
        editshopexperiencewidget.ui \
        editshopitemwidget.ui \
        itemwidget.ui \
        kitswidget.ui \
        mainwindow.ui \
        sellstoreitemwidget.ui \
        sellstorewidget.ui \
        shopstorewidget.ui

RESOURCES += qdarkstyle/style.qrc \
    appstyle.qrc \
    appstyle.qrc

INCLUDEPATH += openssl

RC_ICONS += icons/gamingogs_esports_logo_v2_6aW_icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
