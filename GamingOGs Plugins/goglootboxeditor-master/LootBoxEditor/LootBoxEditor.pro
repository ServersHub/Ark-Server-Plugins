#-------------------------------------------------
#
# Project created by QtCreator 2020-02-09T19:20:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LootBoxEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -lws2_32

QMAKE_LFLAGS += -static -lpthread

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17
CONFIG += static
CONFIG += staticlib

SOURCES += \
    beacon_block_widget.cpp \
    box_widget.cpp \
    bulk_add_dino_widget.cpp \
    bulk_add_item_widget.cpp \
    command_block_widget.cpp \
    create_block_node_widget.cpp \
    dino_block_widget.cpp \
    dino_widget.cpp \
    internal/beacon_block.cpp \
    internal/box.cpp \
    internal/command_block.cpp \
    internal/dino.cpp \
    internal/dino_block.cpp \
    internal/item.cpp \
    internal/item_block.cpp \
    internal/mysql_configuration.cpp \
    internal/node.cpp \
    internal/points_block.cpp \
    item_block_widget.cpp \
    item_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    mysql_widget.cpp \
    points_block_widget.cpp

HEADERS += \
    beacon_block_widget.h \
    box_widget.h \
    bulk_add_dino_widget.h \
    bulk_add_item_widget.h \
    command_block_widget.h \
    create_block_node_widget.h \
    dino_block_widget.h \
    dino_widget.h \
    internal/beacon_block.h \
    internal/box.h \
    internal/command_block.h \
    internal/dino.h \
    internal/dino_block.h \
    internal/item.h \
    internal/item_block.h \
    internal/json.hpp \
    internal/json_utils.h \
    internal/mysql_configuration.h \
    internal/node.h \
    internal/points_block.h \
    item_block_widget.h \
    item_widget.h \
    mainwindow.h \
    mysql_widget.h \
    points_block_widget.h

FORMS += \
    beacon_block_widget.ui \
    box_widget.ui \
    bulk_add_dino_widget.ui \
    bulk_add_item_widget.ui \
    command_block_widget.ui \
    create_block_node_widget.ui \
    dino_block_widget.ui \
    dino_widget.ui \
    item_block_widget.ui \
    item_widget.ui \
    mainwindow.ui \
    mysql_widget.ui \
    points_block_widget.ui

RESOURCES += qdarkstyle/style.qrc \
    appstyle.qrc

RC_ICONS += icons/gamingogs_esports_logo_v2_6aW_icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
