#include "addressing_mode_helpers.hpp"



struct BPL_Relative_Expectations
{
    bool     negative_flag;
    uint16_t program_counter;
};

using BPLRelative     = BPL<Relative, BPL_Relative_Expectations, 2>;
using BPLRelativeMode = ParameterizedInstructionExecutorTestFixture<BPLRelative>;


void StoreTestValueRelativeAddress(InstructionExecutorTestFixture &fixture, const BPLRelative &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.offset;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const BPLRelative &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.negative_flag);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BPLRelative                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveRelativeValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BPL_Relative_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.negative_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BPL, Relative> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BPL, AddressMode_e::Relative) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( instruction.address.offset ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const BPLRelative                    &)
{
    // No memory affected.
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BPLRelative                    &)
{
    // No memory affected
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor &executor,
                                       const BPLRelative         &instruction)
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
                                             const BPLRelative                    &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    // A page boundary is crossed if the two addresses are on different pages.
    bool     page_boundary_is_crossed = InstructionExecutorTestFixture::AddressesAreOnDifferentPages(calculated_offset, next_instruction_address);
    bool     branch_taken = !fixture.executor.registers().GetFlag(FLAGS6502::N);
    uint32_t extra_cycle_count = 0;

    extra_cycle_count += page_boundary_is_crossed;
    extra_cycle_count += branch_taken;

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<BPLRelative> BPLRelativeModeTestValues {
BPLRelative{
    // Beginning of a page.
    // No branching (N is set).
    Relative().address(0x4000).signed_offset(0x00),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x4000},
        .final = {
            .negative_flag = true,
            .program_counter = 0x4002
        }}
},
BPLRelative{
    // End of a page.
    // No branching (N is set).
    Relative().address(0x40FE).signed_offset(0x00),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x40FE},
        .final = {
            .negative_flag = true,
            .program_counter = 0x4100
        }}
},
BPLRelative{
    // Crossing over a page.
    // No branching (N is set).
    Relative().address(0x40FF).signed_offset(0x00),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = true,
            .program_counter = 0x40FF},
        .final = {
            .negative_flag = true,
            .program_counter = 0x4101
        }}
},
BPLRelative{
    // Beginning of a page.
    // Branching (N is clear).
    // No offset
    Relative().address(0x4000).signed_offset(0x00),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x4000},
        .final = {
            .negative_flag = false,
            .program_counter = 0x4002
        }}
},
BPLRelative{
    // Beginning of a page.
    // Branching (N is clear).
    // -1 offset
    Relative().address(0x4000).signed_offset(0xFF),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x4000},
        .final = {
            .negative_flag = false,
            .program_counter = 0x4001
        }}
},
BPLRelative{
    // End of a page
    // Branching (N is clear).
    // Offset goes back to the previous page.
    Relative().address(0x40FE).signed_offset(0xFE),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x40FE},
        .final = {
            .negative_flag = false,
            .program_counter = 0x40FE
        }}
},
BPLRelative{
    // Crossing over a page.
    // Branching (N is clear).
    Relative().address(0x40FF).signed_offset(0x7F),
    BPLRelative::Requirements{
        .initial = {
            .negative_flag = false,
            .program_counter = 0x40FF},
        .final = {
            .negative_flag = false,
            .program_counter = 0x4180
        }}
}
};

TEST_P(BPLRelativeMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BranchOnMinusRelativeAtVariousAddresses,
                         BPLRelativeMode,
                         testing::ValuesIn(BPLRelativeModeTestValues) );
