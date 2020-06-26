#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct BCS_Relative_Expectations
{
    StatusExpectation carry_flag;
    uint16_t          program_counter;
};

using BCSRelative     = BCS<Relative, BCS_Relative_Expectations, 2>;
using BCSRelativeMode = ParameterizedInstructionExecutorTestFixture<BCSRelative>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BCSRelative                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Relative,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.offset;

    //fixture.r.program_counter = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(instruction_param.requirements.initial.carry_flag.status_flag,
                      instruction_param.requirements.initial.carry_flag.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BCS_Relative_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.GetFlag(expectations.carry_flag.status_flag), Eq(expectations.carry_flag.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BCS, Relative> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BCS, AddressMode_e::Relative) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( instruction.address.offset ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const BCSRelative                    &)
{
    // No memory affected.
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BCSRelative                    &)
{
    // No memory affected
}

template<AbstractInstruction_e TOperation>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor               &executor,
                                       const Instruction<TOperation, Relative> &instruction)
{
    // To an offset value from the FOLLOWING instruction!
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    return executor.registers().program_counter == calculated_offset;
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const BCSRelative                    &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    // A page boundary is crossed if the two addresses are on different pages.
    bool     page_boundary_is_crossed = InstructionExecutorTestFixture::AddressesAreOnDifferentPages(calculated_offset, next_instruction_address);
    bool     branch_taken = fixture.executor.registers().GetFlag(FLAGS6502::C);
    uint32_t extra_cycle_count = 0;

    extra_cycle_count += page_boundary_is_crossed;
    extra_cycle_count += branch_taken;

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<BCSRelative> BCSRelativeModeTestValues {
BCSRelative{
    // Beginning of a page.
    // No branching (C is clear).
    Relative().address(0x4000).signed_offset(0x00),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(false),
            .program_counter = 0x4000},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(false),
            .program_counter = 0x4002
        }}
},
BCSRelative{
    // End of a page.
    // No branching (C is clear).
    Relative().address(0x40FE).signed_offset(0x00),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(false),
            .program_counter = 0x40FE},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(false),
            .program_counter = 0x4100
        }}
},
BCSRelative{
    // Crossing over a page.
    // No branching (C is clear).
    Relative().address(0x40FF).signed_offset(0x00),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(false),
            .program_counter = 0x40FF},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(false),
            .program_counter = 0x4101
        }}
},
BCSRelative{
    // Beginning of a page.
    // Branching (C is set).
    // No offset
    Relative().address(0x4000).signed_offset(0x00),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x4000},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x4002
        }}
},
BCSRelative{
    // Beginning of a page.
    // Branching (C is set).
    // -1 offset
    Relative().address(0x4000).signed_offset(0xFF),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x4000},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x4001
        }}
},
BCSRelative{
    // End of a page
    // Branching (C is set).
    // Offset goes back to the previous page.
    Relative().address(0x40FE).signed_offset(0xFE),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x40FE},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x40FE
        }}
},
BCSRelative{
    // Crossing over a page.
    // Branching (C is set).
    Relative().address(0x40FF).signed_offset(0x7F),
    BCSRelative::Requirements{
        .initial = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x40FF},
        .final = {
            .carry_flag = StatusExpectation().flag(FLAGS6502::C).value(true),
            .program_counter = 0x4180
        }}
}
};

TEST_P(BCSRelativeMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BranchOnCarrySetRelativeAtVariousAddresses,
                         BCSRelativeMode,
                         testing::ValuesIn(BCSRelativeModeTestValues) );
