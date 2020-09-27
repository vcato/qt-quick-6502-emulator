#include "addressing_mode_helpers.hpp"



struct BVS_Relative_Expectations
{
    bool     overflow_flag;
    uint16_t program_counter;
};

using BVSRelative     = BVS<Relative, BVS_Relative_Expectations, 2>;
using BVSRelativeMode = ParameterizedInstructionExecutorTestFixture<BVSRelative>;


void StoreTestValueRelativeAddress(InstructionExecutorTestFixture &fixture, const BVSRelative &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.offset;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const BVSRelative &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.overflow_flag);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BVSRelative                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveRelativeValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BVS_Relative_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.overflow_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BVS, Relative> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BVS, AddressMode_e::Relative) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( instruction.address.offset ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const BVSRelative                    &)
{
    // No memory affected.
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BVSRelative                    &)
{
    // No memory affected
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor               &executor,
                                       const BVSRelative                    &instruction)
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
                                             const BVSRelative                    &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint16_t next_instruction_address = instruction.address.instruction_address +
                                        instruction.address.operand_byte_count + 1;
    uint16_t calculated_offset = InstructionExecutorTestFixture::SignedOffsetFromAddress(next_instruction_address,
                                                                                         instruction.address.offset);

    // A page boundary is crossed if the two addresses are on different pages.
    bool     page_boundary_is_crossed = InstructionExecutorTestFixture::AddressesAreOnDifferentPages(calculated_offset, next_instruction_address);
    bool     branch_taken = fixture.executor.registers().GetFlag(FLAGS6502::V);
    uint32_t extra_cycle_count = 0;

    extra_cycle_count += page_boundary_is_crossed;
    extra_cycle_count += branch_taken;

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<BVSRelative> BVSRelativeModeTestValues {
BVSRelative{
    // Beginning of a page.
    // No branching (V is clear).
    Relative().address(0x4000).signed_offset(0x00),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = false,
            .program_counter = 0x4000},
        .final = {
            .overflow_flag = false,
            .program_counter = 0x4002
        }}
},
BVSRelative{
    // End of a page.
    // No branching (V is clear).
    Relative().address(0x40FE).signed_offset(0x00),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = false,
            .program_counter = 0x40FE},
        .final = {
            .overflow_flag = false,
            .program_counter = 0x4100
        }}
},
BVSRelative{
    // Crossing over a page.
    // No branching (V is clear).
    Relative().address(0x40FF).signed_offset(0x00),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = false,
            .program_counter = 0x40FF},
        .final = {
            .overflow_flag = false,
            .program_counter = 0x4101
        }}
},
BVSRelative{
    // Beginning of a page.
    // Branching (V is set).
    // No offset
    Relative().address(0x4000).signed_offset(0x00),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = true,
            .program_counter = 0x4000},
        .final = {
            .overflow_flag = true,
            .program_counter = 0x4002
        }}
},
BVSRelative{
    // Beginning of a page.
    // Branching (V is set).
    // -1 offset
    Relative().address(0x4000).signed_offset(0xFF),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = true,
            .program_counter = 0x4000},
        .final = {
            .overflow_flag = true,
            .program_counter = 0x4001
        }}
},
BVSRelative{
    // End of a page
    // Branching (V is set).
    // Offset goes back to the previous page.
    Relative().address(0x40FE).signed_offset(0xFE),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = true,
            .program_counter = 0x40FE},
        .final = {
            .overflow_flag = true,
            .program_counter = 0x40FE
        }}
},
BVSRelative{
    // Crossing over a page.
    // Branching (V is set).
    Relative().address(0x40FF).signed_offset(0x7F),
    BVSRelative::Requirements{
        .initial = {
            .overflow_flag = true,
            .program_counter = 0x40FF},
        .final = {
            .overflow_flag = true,
            .program_counter = 0x4180
        }}
}
};

TEST_P(BVSRelativeMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BranchOnOverflowSetRelativeAtVariousAddresses,
                         BVSRelativeMode,
                         testing::ValuesIn(BVSRelativeModeTestValues) );
