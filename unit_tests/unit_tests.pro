# Use a relative path so we can just use it as a submodule from git.
GOOGLETEST_DIR = ../googletest

include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

HEADERS += \
        tst_testcaseone.h

SOURCES += \
        main.cpp
