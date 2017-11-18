HEADERS += Hangman.h
HEADERS += Dialog.h

SOURCES += main.cc
SOURCES += Hangman.cc
SOURCES += Dialog.cc

CONFIG += debug

QMAKE_CXXFLAGS += -std=c++17

QT += widgets
