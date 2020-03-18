#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"
#include "opcodes.hpp"

using namespace testing;

TEST_F(LoadAccumulatorWithMemory, AbsoluteIndexedWithY)
{
    // Setup
    Param_AbsoluteIndexedWithY param { .instruction_address = 0x8000,
                                       .address_to_load_from = 0xA000,
                                       .value_to_load = 6,
                                       .y_register    = 32 };

    setup_LDA_AbsoluteIndexedWithY(param);

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(param.instruction_address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteYIndexed) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1 ), Eq( loByteOf(param.address_to_load_from) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 2 ), Eq( hiByteOf(param.address_to_load_from) ));
    EXPECT_THAT(fakeMemory.at(param.address_to_load_from + param.y_register), Eq(param.value_to_load));
    EXPECT_THAT(executor.registers().a, Eq(0U));
    EXPECT_THAT(executor.registers().y, Eq(param.y_register));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(param.instruction_address + 3));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    EXPECT_THAT(executor.registers().a, Eq(param.value_to_load));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(false));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(false));
}
