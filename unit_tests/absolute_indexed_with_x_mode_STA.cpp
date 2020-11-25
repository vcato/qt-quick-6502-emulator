#include "addressing_mode_helpers.hpp"



struct STA_AbsoluteXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
};

using STAAbsoluteXIndexed     = STA<AbsoluteXIndexed, STA_AbsoluteXIndexed_Expectations, 5>;
using STAAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<STAAbsoluteXIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAAbsoluteXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.final.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAAbsoluteXIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAAbsoluteXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.x ), Eq( instruction.requirements.initial.a ));
}


static const std::vector<STAAbsoluteXIndexed> STAAbsoluteXIndexedModeTestValues {
STAAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0 },
        .final = {
            .a = 6,
            .x = 0
        }}
},
STAAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0088).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 5 },
        .final = {
            .a = 6,
            .x = 5
        }}
},
STAAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0 },
        .final = {
            .a = 6,
            .x = 0
        }}
},
STAAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0 },
        .final = {
            .a = 6,
            .x = 0
        }}
},
STAAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0 },
        .final = {
            .a = 6,
            .x = 0
        }}
},
STAAbsoluteXIndexed{
    // Loading a zero affects the Z flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0 },
        .final = {
            .a = 0xFF,
            .x = 0
        }}
},
STAAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    STAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x10,
            .x = 0 },
        .final = {
            .a = 0x10,
            .x = 0
        }}
}
};

TEST_P(STAAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorAbsoluteXIndexedAtVariousAddresses,
                         STAAbsoluteXIndexedMode,
                         testing::ValuesIn(STAAbsoluteXIndexedModeTestValues) );
