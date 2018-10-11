#-------------------------------------------------
#
# Project created by QtCreator 2018-06-08T08:40:09
#
#-------------------------------------------------

QT       += core gui

QT      += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test3
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    comandar.cpp \
    consultar.cpp \
    enviar_mensaje.cpp \
    estadored.cpp \
    gen_eolico.cpp \
    LACAN_SEND.cpp \
    LACAN_REC.cpp \
    tiempo.cpp \
    LACAN_PRO.cpp \
    better_serialfunc.cpp \
    addnewdevdialog.cpp

HEADERS += \
        mainwindow.h \
    comandar.h \
    consultar.h \
    enviar_mensaje.h \
    estadored.h \
    gen_eolico.h \
    LACAN_SEND.h \
    LACAN_REC.h \
    tiempo.h \
    LACAN_PRO.h \
    pc.h \
    lacan_detect.h \
    better_serialfunc.h \
    addnewdevdialog.h \
    limtes.h \
    limites.h

FORMS += \
        mainwindow.ui \
    comandar.ui \
    consultar.ui \
    enviar_mensaje.ui \
    estadored.ui \
    gen_eolico.ui \
    addnewdevdialog.ui

RESOURCES += \
    imagenes.qrc


