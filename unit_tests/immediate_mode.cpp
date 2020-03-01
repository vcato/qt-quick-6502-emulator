#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"

using namespace testing;


TEST_F(LoadAccumulatorWithMemory, Immediate)
{
    // Setup
    const addressType address = 0x0000;
    const uint8_t     value_to_load = 6;

    loadInstructionIntoMemory(AbstractInstruction_e::LDA, AddressMode_e::Immediate, address);
    fakeMemory[address + 1] = value_to_load;

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::Immediate) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq(value_to_load));
    EXPECT_THAT(executor.registers().a, Eq(0U));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(2U));
    EXPECT_THAT(executor.registers().a, Eq(value_to_load));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));
}
