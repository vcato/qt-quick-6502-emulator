#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "olc6502.hpp"

using namespace testing;

bool Carry(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::C;
}

bool Zero(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::Z;
}

bool Interrupt(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::I;
}

bool Decimal(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::D;
}

bool Break(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::B;
}

bool Overflow(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::V;
}

bool Negative(uint8_t status_value)
{
    return status_value & olc6502::FLAGS6502::N;
}

TEST(CPU, ResetSetsProcessorToKnownState)
{
    olc6502 cpu;

    cpu.reset();

    // Official behaviors
    EXPECT_THAT(cpu.a(), Eq(0)) << "Accumulator is not zero";
    EXPECT_THAT(cpu.x(), Eq(0)) << "X register is not zero";
    EXPECT_THAT(cpu.y(), Eq(0)) << "Y register is not zero";
    EXPECT_THAT(cpu.stackPointer(), Eq(0xFD)) << "Stack Pointer register is not 0xFD";
    EXPECT_THAT(Carry(cpu.status()), Eq(false)) << "Carry flag is not clear";
    EXPECT_THAT(Zero(cpu.status()), Eq(false)) << "Zero flag is not clear";
    EXPECT_THAT(Interrupt(cpu.status()), Eq(false)) << "Interrupt flag is not clear";
    EXPECT_THAT(Decimal(cpu.status()), Eq(false)) << "Decimal flag is not clear";
    EXPECT_THAT(Break(cpu.status()), Eq(false)) << "Break flag is not clear";
    EXPECT_THAT(Overflow(cpu.status()), Eq(false)) << "Overflow flag is not clear";
    EXPECT_THAT(Negative(cpu.status()), Eq(false)) << "Negative flag is not clear";

    // cpu.pc() is read from memory address 0xFFFC  Add test

    // Extra, implementation-dependent expectations
    EXPECT_THAT(cpu.clockTicks(), Eq(0)) << "Number of clock ticks should be zero";
    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should not be immediately completing an instruction";
}

TEST(CPU, ClockTickIncrementsClockCount)
{
    olc6502 cpu;

    cpu.reset();

    EXPECT_THAT(cpu.clockTicks(), Eq(0)) << "Number of clock ticks is not zero";

    cpu.clock();

    EXPECT_THAT(cpu.clockTicks(), Eq(1)) << "Only one clock cycle is supposed to occur";
}

TEST(CPU, ResetTakesDeterminateNumberOfCycles)
{
    olc6502 cpu;

    cpu.reset();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should set complete() to false";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(false)) << "A CPU reset should take 8 cycles";

    cpu.clock();

    EXPECT_THAT(cpu.complete(), Eq(true)) << "A CPU reset should take 8 cycles";
}
