#include "addressing_mode_helpers.hpp"



struct STA_AbsoluteYIndexed_Expectations
{
    uint8_t a;
    uint8_t y;
};

using STAAbsoluteYIndexed     = STA<AbsoluteYIndexed, STA_AbsoluteYIndexed_Expectations, 5>;
using STAAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<STAAbsoluteYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAAbsoluteYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAAbsoluteYIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAAbsoluteYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::AbsoluteYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.y ), Eq( instruction.requirements.initial.a ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const STAAbsoluteYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<STAAbsoluteYIndexed> STAAbsoluteYIndexedModeTestValues {
STAAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 0 },
        .final = {
            .a = 6,
            .y = 0
        }}
},
STAAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x81,
            .y = 5 },
        .final = {
            .a = 0x81,
            .y = 5
        }}
},
STAAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0 },
        .final = {
            .a = 0,
            .y = 0
        }}
},
STAAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 0 },
        .final = {
            .a = 0xFF,
            .y = 0
        }}
},
STAAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 0 },
        .final = {
            .a = 0x80,
            .y = 0
        }}
},
// Index to the top of the page
STAAbsoluteYIndexed{
    AbsoluteYIndexed().address(0x8000).value(0xA001),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x7F,
            .y = 0xFE },
        .final = {
            .a = 0x7F,
            .y = 0xFE
        }}
},
// Index into the next page
STAAbsoluteYIndexed{
    AbsoluteYIndexed().address(0x8000).value(0xA001),
    STAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .y = 0xFF },
        .final = {
            .a = 0x01,
            .y = 0xFF
        }}
}
};

TEST_P(STAAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorAbsoluteYIndexedAtVariousAddresses,
                         STAAbsoluteYIndexedMode,
                         testing::ValuesIn(STAAbsoluteYIndexedModeTestValues) );
