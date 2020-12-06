#include "addressing_mode_helpers.hpp"



struct STX_Absolute_Expectations
{
    uint8_t x;
};

using STXAbsolute     = STX<Absolute, STX_Absolute_Expectations, 4>;
using STXAbsoluteMode = ParameterizedInstructionExecutorTestFixture<STXAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STXAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.x;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STXAbsolute &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STXAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STX_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STX, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STX, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STXAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.x ));
}


static const std::vector<STXAbsolute> STXAbsoluteModeTestValues {
STXAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 0 },
        .final = {
            .x = 0
        }}
},
STXAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 6 },
        .final = {
            .x = 6
        }}
},
STXAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 6 },
        .final = {
            .x = 6
        }}
},
STXAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 6 },
        .final = {
            .x = 6
        }}
},
STXAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 6 },
        .final = {
            .x = 6
        }}
},
STXAbsolute{
    Absolute().address(0x8000).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 0xA0 },
        .final = {
            .x = 0xA0
        }}
},
STXAbsolute{
    Absolute().address(0x8000).value(0xA000),
    STXAbsolute::Requirements{
        .initial = {
            .x = 0xFF },
        .final = {
            .x = 0xFF
        }}
}
};

TEST_P(STXAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreXAbsoluteAtVariousAddresses,
                         STXAbsoluteMode,
                         testing::ValuesIn(STXAbsoluteModeTestValues) );
