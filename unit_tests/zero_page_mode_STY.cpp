#include "addressing_mode_helpers.hpp"



struct STY_ZeroPage_Expectations
{
    uint8_t y;
};

using STYZeroPage     = STY<ZeroPage, STY_ZeroPage_Expectations, 3>;
using STYZeroPageMode = ParameterizedInstructionExecutorTestFixture<STYZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STYZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.y;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STYZeroPage &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STYZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STY_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STY, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STY, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STYZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.y));
}


static const std::vector<STYZeroPage> STYZeroPageModeTestValues {
STYZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    STYZeroPage::Requirements{
        .initial = {
            .y = 0 },
        .final = {
            .y = 0
        }}
},
STYZeroPage{
    // One before the end of a page
    ZeroPage().address(0x00FE).zp_address(6),
    STYZeroPage::Requirements{
        .initial = {
            .y = 0xFF },
        .final = {
            .y = 0xFF
        }}
},
STYZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x00FF).zp_address(6),
    STYZeroPage::Requirements{
        .initial = {
            .y = 0x80 },
        .final = {
            .y = 0x80
        }}
},
STYZeroPage{
    ZeroPage().address(0x8000).zp_address(16),
    STYZeroPage::Requirements{
        .initial = {
            .y = 0x7F },
        .final = {
            .y = 0x7F
        }}
},
STYZeroPage{
    ZeroPage().address(0x8000).zp_address(0xFF),
    STYZeroPage::Requirements{
        .initial = {
            .y = 1 },
        .final = {
            .y = 1
        }}
}
};

TEST_P(STYZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreYZeroPageAtVariousAddresses,
                         STYZeroPageMode,
                         testing::ValuesIn(STYZeroPageModeTestValues) );
