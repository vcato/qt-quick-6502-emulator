#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct BCC_Relative_Expectations
{
    bool     carry_flag;
    uint16_t program_counter;
};

using BCCRelative     = BCC<Relative, BCC_Relative_Expectations, 2>;
using BCCRelativeMode = ParameterizedInstructionExecutorTestFixture<BCCRelative>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BCCRelative                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Relative,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.offset;

    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.carry_flag);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BCC_Relative_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.carry_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BCC, Relative> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BCC, AddressMode_e::Relative) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( instruction.address.offset ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const BCCRelative                    &)
{
    // No memory affected.
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BCCRelative                    &)
{
    // No memory affected
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor &executor,
                                       const BCCRelative         &instruction)
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
                                             const BCCRelative                    &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    // A page boundary is crossed if the two addresses are on different pages.
    bool     page_boundary_is_crossed = InstructionExecutorTestFixture::AddressesAreOnDifferentPages(calculated_offset, next_instruction_address);
    bool     branch_taken = !fixture.executor.registers().GetFlag(FLAGS6502::C);
    uint32_t extra_cycle_count = 0;

    extra_cycle_count += page_boundary_is_crossed;
    extra_cycle_count += branch_taken;

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<BCCRelative> BCCRelativeModeTestValues {
BCCRelative{
    // Beginning of a page.
    // No branching (C is set).
    Relative().address(0x4000).signed_offset(0x00),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = true,
            .program_counter = 0x4000},
        .final = {
            .carry_flag = true,
            .program_counter = 0x4002
        }}
},
BCCRelative{
    // End of a page.
    // No branching (C is set).
    Relative().address(0x40FE).signed_offset(0x00),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = true,
            .program_counter = 0x40FE},
        .final = {
            .carry_flag = true,
            .program_counter = 0x4100
        }}
},
BCCRelative{
    // Crossing over a page.
    // No branching (C is set).
    Relative().address(0x40FF).signed_offset(0x00),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = true,
            .program_counter = 0x40FF},
        .final = {
            .carry_flag = true,
            .program_counter = 0x4101
        }}
},
BCCRelative{
    // Beginning of a page.
    // Branching (C is clear).
    // No offset
    Relative().address(0x4000).signed_offset(0x00),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = false,
            .program_counter = 0x4000},
        .final = {
            .carry_flag = false,
            .program_counter = 0x4002
        }}
},
BCCRelative{
    // Beginning of a page.
    // Branching (C is clear).
    // -1 offset
    Relative().address(0x4000).signed_offset(0xFF),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = false,
            .program_counter = 0x4000},
        .final = {
            .carry_flag = false,
            .program_counter = 0x4001
        }}
},
BCCRelative{
    // End of a page
    // Branching (C is clear).
    // Offset goes back to the previous page.
    Relative().address(0x40FE).signed_offset(0xFE),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = false,
            .program_counter = 0x40FE},
        .final = {
            .carry_flag = false,
            .program_counter = 0x40FE
        }}
},
BCCRelative{
    // Crossing over a page.
    // Branching (C is clear).
    Relative().address(0x40FF).signed_offset(0x7F),
    BCCRelative::Requirements{
        .initial = {
            .carry_flag = false,
            .program_counter = 0x40FF},
        .final = {
            .carry_flag = false,
            .program_counter = 0x4180
        }}
}
};

TEST_P(BCCRelativeMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BranchOnCarryClearRelativeAtVariousAddresses,
                         BCCRelativeMode,
                         testing::ValuesIn(BCCRelativeModeTestValues) );
