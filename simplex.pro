TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    my_conio.c \
    old_main.c

HEADERS += \
    solver.h \
    my_conio.h
