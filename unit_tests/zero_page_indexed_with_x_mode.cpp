#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"
#include "opcodes.hpp"

using namespace testing;

TEST_F(LoadAccumulatorWithMemory, ZeroPageIndexedWithX)
{
    // Setup
    const addressType address = 0x8000;
    const uint8_t     zero_page_address_to_load = 6;
    const uint8_t     zero_page_data    = 128;
    const uint8_t     x_register_value = 3;

    loadInstructionIntoMemory(AbstractInstruction_e::LDA, AddressMode_e::ZeroPageXIndexed, address);
    fakeMemory[address + 1] = zero_page_address_to_load;
    fakeMemory[zero_page_address_to_load + x_register_value] = zero_page_data;
    executor.registers().x = x_register_value;

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq(zero_page_address_to_load));
    EXPECT_THAT(fakeMemory.at(zero_page_address_to_load + x_register_value), Eq(zero_page_data));
    EXPECT_THAT(executor.registers().a, Eq(0U));
    EXPECT_THAT(executor.registers().x, Eq(x_register_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    EXPECT_THAT(executor.registers().a, Eq(zero_page_data));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(true));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));
}
