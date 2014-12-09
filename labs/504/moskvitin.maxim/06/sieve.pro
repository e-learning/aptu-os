TEMPLATE = app
CONFIG += console
CONFIG += thread
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11 -pthread -O2

HEADERS += bit_array.h
SOURCES += bit_array.cpp multith.cpp unith.cpp

