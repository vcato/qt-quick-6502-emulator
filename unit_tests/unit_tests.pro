# Use a relative path so we can just use it as a submodule from git.
#GOOGLETEST_DIR = ../googletest

include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt

QT += quick

HEADERS += \
    InstructionExecutorTestFixture.hpp \
    instruction_checks.hpp \
    instruction_definitions.hpp \
    instruction_helpers.hpp

SOURCES += \
        6502_tests.cpp \
        absolute_indexed_with_x_mode_LDA.cpp \
        absolute_indexed_with_x_mode_LDY.cpp \
        absolute_indexed_with_y_mode_LDA.cpp \
        absolute_indexed_with_y_mode_LDX.cpp \
        absolute_mode_LDA.cpp \
        absolute_mode_LDX.cpp \
        absolute_mode_LDY.cpp \
        accumulator_mode.cpp \
        immediate_mode_LDA.cpp \
        immediate_mode_LDX.cpp \
        immediate_mode_LDY.cpp \
        implied_mode.cpp \
        indirect_y_indexed_LDA.cpp \
        instruction_executor_tests.cpp \
        registers_tests.cpp \
        relative_mode.cpp \
        x_indexed_indirect_LDA.cpp \
        zero_page_indexed_with_x_mode_LDA.cpp \
        main.cpp \
        zero_page_indexed_with_x_mode_LDY.cpp \
        zero_page_indexed_with_y_mode_LDX.cpp \
        zero_page_mode_LDA.cpp \
        zero_page_mode_LDX.cpp \
        zero_page_mode_LDY.cpp

# Generated by the "Add Library..." right mouse menu option.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../emulator/release/ -lemulator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../emulator/debug/ -lemulator
else:unix: LIBS += -L$$OUT_PWD/../emulator/ -lemulator

INCLUDEPATH += $$PWD/../emulator
DEPENDPATH += $$PWD/../emulator

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../emulator/release/libemulator.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../emulator/debug/libemulator.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../emulator/release/emulator.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../emulator/debug/emulator.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../emulator/libemulator.a

DISTFILES += \
    PLAN.md
