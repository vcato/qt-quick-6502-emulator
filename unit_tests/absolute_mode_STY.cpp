#include "addressing_mode_helpers.hpp"



struct STY_Absolute_Expectations
{
    uint8_t y;
};

using STYAbsolute     = STY<Absolute, STY_Absolute_Expectations, 4>;
using STYAbsoluteMode = ParameterizedInstructionExecutorTestFixture<STYAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STYAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.y;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STYAbsolute &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STYAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STY_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STY, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STY, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STYAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.y ));
}


static const std::vector<STYAbsolute> STYAbsoluteModeTestValues {
STYAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 0 },
        .final = {
            .y = 0
        }}
},
STYAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 6 },
        .final = {
            .y = 6
        }}
},
STYAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 6 },
        .final = {
            .y = 6
        }}
},
STYAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 6 },
        .final = {
            .y = 6
        }}
},
STYAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 6 },
        .final = {
            .y = 6
        }}
},
STYAbsolute{
    Absolute().address(0x8000).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 0xA0 },
        .final = {
            .y = 0xA0
        }}
},
STYAbsolute{
    Absolute().address(0x8000).value(0xA000),
    STYAbsolute::Requirements{
        .initial = {
            .y = 0xFF },
        .final = {
            .y = 0xFF
        }}
}
};

TEST_P(STYAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreYAbsoluteAtVariousAddresses,
                         STYAbsoluteMode,
                         testing::ValuesIn(STYAbsoluteModeTestValues) );
