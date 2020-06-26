#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct BMI_Relative_Expectations
{
    bool     negative_flag;
    uint16_t program_counter;
};

using BMIRelative     = BMI<Relative, BMI_Relative_Expectations, 2>;
using BMIRelativeMode = ParameterizedInstructionExecutorTestFixture<BMIRelative>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BMIRelative                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Relative,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.offset;

    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.negative_flag);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BMI_Relative_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.negative_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BMI, Relative> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BMI, AddressMode_e::Relative) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( instruction.address.offset ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const BMIRelative                    &)
{
    // No memory affected.
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BMIRelative                    &)
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
                                             const BMIRelative                    &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    // A page boundary is crossed if the two addresses are on different pages.
    bool     page_boundary_is_crossed = InstructionExecutorTestFixture::AddressesAreOnDifferentPages(calculated_offset, next_instruction_address);
    bool     branch_taken = fixture.executor.registers().GetFlag(FLAGS6502::N);
    uint32_t extra_cycle_count = 0;

    extra_cycle_count += page_boundary_is_crossed;
    extra_cycle_count += branch_taken;

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<BMIRelative> BMIRelativeModeTestValues {
BMIRelative{
    // Beginning of a page.
    // No branching (N is clear).
    Relative().address(0x4000).signed_offset(0x00),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x4000},
        .final = {
            .negative_flag = false,
            .program_counter = 0x4002
        }}
},
BMIRelative{
    // End of a page.
    // No branching (N is clear).
    Relative().address(0x40FE).signed_offset(0x00),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x40FE},
        .final = {
            .negative_flag = false,
            .program_counter = 0x4100
        }}
},
BMIRelative{
    // Crossing over a page.
    // No branching (N is clear).
    Relative().address(0x40FF).signed_offset(0x00),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x40FF},
        .final = {
            .negative_flag = false,
            .program_counter = 0x4101
        }}
},
BMIRelative{
    // Beginning of a page.
    // Branching (N is set).
    // No offset
    Relative().address(0x4000).signed_offset(0x00),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x4000},
        .final = {
            .negative_flag = true,
            .program_counter = 0x4002
        }}
},
BMIRelative{
    // Beginning of a page.
    // Branching (N is set).
    // -1 offset
    Relative().address(0x4000).signed_offset(0xFF),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x4000},
        .final = {
            .negative_flag = true,
            .program_counter = 0x4001
        }}
},
BMIRelative{
    // End of a page
    // Branching (N is set).
    // Offset goes back to the previous page.
    Relative().address(0x40FE).signed_offset(0xFE),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x40FE},
        .final = {
            .negative_flag = true,
            .program_counter = 0x40FE
        }}
},
BMIRelative{
    // Crossing over a page.
    // Branching (N is set).
    Relative().address(0x40FF).signed_offset(0x7F),
    BMIRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x40FF},
        .final = {
            .negative_flag = true,
            .program_counter = 0x4180
        }}
}
};

TEST_P(BMIRelativeMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BranchOnMinusRelativeAtVariousAddresses,
                         BMIRelativeMode,
                         testing::ValuesIn(BMIRelativeModeTestValues) );
