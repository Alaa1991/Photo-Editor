#-------------------------------------------------
#
# Project created by QtCreator 2023-09-16T16:50:13
#
#-------------------------------------------------
#QMAKE_CXXFLAGS += -std=c++11
QT       += core gui
CONFIG += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoEditorV2
TEMPLATE = app


SOURCES += main.cpp\
    DrawingManager.cpp \
        mainwindow.cpp \
    customgraphicsview.cpp

HEADERS  += mainwindow.h \
    DrawingManager.h \
    EdgeEnum.h \
    TextTool.h \
    customgraphicsview.h

FORMS    += mainwindow.ui

DISTFILES += \
    C:/Users/ALAAHOME/Downloads/arrow-right.png \
    C:/Users/ALAAHOME/Downloads/circle.png \
    C:/Users/ALAAHOME/Downloads/color-palette.png \
    C:/Users/ALAAHOME/Downloads/fire-symbol.png \
    C:/Users/ALAAHOME/Downloads/greyscale.png \
    C:/Users/ALAAHOME/Downloads/pencil.png \
    C:/Users/ALAAHOME/Downloads/rounded-rectangle.png \
    C:/Users/ALAAHOME/Downloads/star.png \
    C:/Users/ALAAHOME/Downloads/text.png \
    C:/Users/ALAAHOME/Downloads/trapezium.png \
    styles.qss

RESOURCES += \
    resources.qrc
