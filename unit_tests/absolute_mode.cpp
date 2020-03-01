#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"
#include "opcodes.hpp"

using namespace testing;

TEST_F(LoadAccumulatorWithMemory, Absolute)
{
    // Setup
    const addressType address = 0x8000;
    const addressType address_to_load_from = 0xA000;
    const uint8_t     value_to_load = 6;

    loadInstructionIntoMemory(AbstractInstruction_e::LDA, AddressMode_e::Absolute, address);
    fakeMemory[address + 1] = loByteOf(address_to_load_from);
    fakeMemory[address + 2] = hiByteOf(address_to_load_from);
    fakeMemory[address_to_load_from] = value_to_load;

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::Absolute) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1 ), Eq( loByteOf(address_to_load_from) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 2 ), Eq( hiByteOf(address_to_load_from) ));
    EXPECT_THAT(fakeMemory.at(address_to_load_from), Eq(value_to_load));
    EXPECT_THAT(executor.registers().a, Eq(0U));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(address + 3));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    EXPECT_THAT(executor.registers().a, Eq(value_to_load));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));
}
