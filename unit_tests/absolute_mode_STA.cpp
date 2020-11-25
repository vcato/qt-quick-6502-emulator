#include "addressing_mode_helpers.hpp"



struct STA_Absolute_Expectations
{
    uint8_t a;
};

using STAAbsolute     = STA<Absolute, STA_Absolute_Expectations, 4>;
using STAAbsoluteMode = ParameterizedInstructionExecutorTestFixture<STAAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.final.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAAbsolute &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.final.a ));
}


static const std::vector<STAAbsolute> STAAbsoluteModeTestValues {
STAAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 0 },
        .final = {
            .a = 0
        }}
},
STAAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 6 },
        .final = {
            .a = 6
        }}
},
STAAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 6 },
        .final = {
            .a = 6
        }}
},
STAAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 6 },
        .final = {
            .a = 6
        }}
},
STAAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 6 },
        .final = {
            .a = 6
        }}
},
STAAbsolute{
    Absolute().address(0x8000).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 0xA0 },
        .final = {
            .a = 0xA0
        }}
},
STAAbsolute{
    Absolute().address(0x8000).value(0xA000),
    STAAbsolute::Requirements{
        .initial = {
            .a = 0xFF },
        .final = {
            .a = 0xFF
        }}
}
};

TEST_P(STAAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorAbsoluteAtVariousAddresses,
                         STAAbsoluteMode,
                         testing::ValuesIn(STAAbsoluteModeTestValues) );
