#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"
#include "opcodes.hpp"

using namespace testing;

TEST_F(LoadAccumulatorWithMemory, IndirectYIndexed)
{
    // Setup
    const addressType address = 0x8000;
    const addressType address_stored_in_zero_page    = 0xC000;
    const uint8_t     zero_page_address_to_load_from = 0xA0;
    const uint8_t     value_to_load = 6;
    const uint8_t     y_register    = 12;

    loadInstructionIntoMemory(AbstractInstruction_e::LDA, AddressMode_e::IndirectYIndexed, address);
    fakeMemory[address + 1] = zero_page_address_to_load_from;
    fakeMemory[zero_page_address_to_load_from ]    = loByteOf(address_stored_in_zero_page);
    fakeMemory[zero_page_address_to_load_from + 1] = hiByteOf(address_stored_in_zero_page);
    fakeMemory[address_stored_in_zero_page + y_register] = value_to_load;
    executor.registers().y = y_register;

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1 ), Eq(zero_page_address_to_load_from));
    EXPECT_THAT(fakeMemory.at( zero_page_address_to_load_from ),    Eq( loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fakeMemory.at( zero_page_address_to_load_from + 1), Eq( hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fakeMemory.at( address_stored_in_zero_page + y_register ), Eq(value_to_load));
    EXPECT_THAT(executor.registers().a, Eq(0U));
    EXPECT_THAT(executor.registers().y, Eq(y_register));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(5U));
    EXPECT_THAT(executor.registers().a, Eq(value_to_load));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));
}
