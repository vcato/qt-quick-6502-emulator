#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"
#include "opcodes.hpp"

using namespace testing;

TEST_F(LoadAccumulatorWithMemory, ZeroPage)
{
    // Setup
    const addressType address = 0x8000;
    const uint8_t     zero_page_address_to_load = 6;
    const uint8_t     zero_page_data = 128;

    loadInstructionIntoMemory(AbstractInstruction_e::LDA, AddressMode_e::ZeroPage, address);
    fakeMemory[address + 1] = zero_page_address_to_load;
    fakeMemory[zero_page_address_to_load] = zero_page_data;

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq(zero_page_address_to_load));
    EXPECT_THAT(fakeMemory.at(zero_page_address_to_load), Eq(zero_page_data));
    EXPECT_THAT(executor.registers().a, Eq(0U));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(3U));
    EXPECT_THAT(executor.registers().a, Eq(zero_page_data));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(true));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));
}
