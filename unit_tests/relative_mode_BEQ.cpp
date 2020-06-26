#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct BEQ_Relative_Expectations
{
    StatusExpectation zero_flag;
    uint16_t          program_counter;
};

using BEQRelative     = BEQ<Relative, BEQ_Relative_Expectations, 2>;
using BEQRelativeMode = ParameterizedInstructionExecutorTestFixture<BEQRelative>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BEQRelative                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Relative,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.offset;

    //fixture.r.program_counter = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(instruction_param.requirements.initial.zero_flag.status_flag,
                      instruction_param.requirements.initial.zero_flag.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BEQ_Relative_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.GetFlag(expectations.zero_flag.status_flag), Eq(expectations.zero_flag.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BEQ, Relative> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BEQ, AddressMode_e::Relative) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( instruction.address.offset ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const BEQRelative                    &)
{
    // No memory affected.
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BEQRelative                    &)
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
                                             const BEQRelative                    &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    // A page boundary is crossed if the two addresses are on different pages.
    bool     page_boundary_is_crossed = InstructionExecutorTestFixture::AddressesAreOnDifferentPages(calculated_offset, next_instruction_address);
    bool     branch_taken = fixture.executor.registers().GetFlag(FLAGS6502::Z);
    uint32_t extra_cycle_count = 0;

    extra_cycle_count += page_boundary_is_crossed;
    extra_cycle_count += branch_taken;

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<BEQRelative> BEQRelativeModeTestValues {
BEQRelative{
    // Beginning of a page.
    // No branching (Z is clear).
    Relative().address(0x4000).signed_offset(0x00),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(false),
            .program_counter = 0x4000},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(false),
            .program_counter = 0x4002
        }}
},
BEQRelative{
    // End of a page.
    // No branching (Z is clear).
    Relative().address(0x40FE).signed_offset(0x00),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(false),
            .program_counter = 0x40FE},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(false),
            .program_counter = 0x4100
        }}
},
BEQRelative{
    // Crossing over a page.
    // No branching (Z is clear).
    Relative().address(0x40FF).signed_offset(0x00),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(false),
            .program_counter = 0x40FF},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(false),
            .program_counter = 0x4101
        }}
},
BEQRelative{
    // Beginning of a page.
    // Branching (Z is set).
    // No offset
    Relative().address(0x4000).signed_offset(0x00),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x4000},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x4002
        }}
},
BEQRelative{
    // Beginning of a page.
    // Branching (Z is set).
    // -1 offset
    Relative().address(0x4000).signed_offset(0xFF),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x4000},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x4001
        }}
},
BEQRelative{
    // End of a page
    // Branching (Z is set).
    // Offset goes back to the previous page.
    Relative().address(0x40FE).signed_offset(0xFE),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x40FE},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x40FE
        }}
},
BEQRelative{
    // Crossing over a page.
    // Branching (Z is set).
    Relative().address(0x40FF).signed_offset(0x7F),
    BEQRelative::Requirements{
        .initial = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x40FF},
        .final = {
            .zero_flag = StatusExpectation().flag(FLAGS6502::Z).value(true),
            .program_counter = 0x4180
        }}
}
};

TEST_P(BEQRelativeMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BranchOnEqualToZeroRelativeAtVariousAddresses,
                         BEQRelativeMode,
                         testing::ValuesIn(BEQRelativeModeTestValues) );
