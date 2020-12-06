#include "addressing_mode_helpers.hpp"



struct STX_ZeroPage_Expectations
{
    uint8_t x;
};

using STXZeroPage     = STX<ZeroPage, STX_ZeroPage_Expectations, 3>;
using STXZeroPageMode = ParameterizedInstructionExecutorTestFixture<STXZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STXZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.x;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STXZeroPage &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STXZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STX_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STX, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STX, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STXZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.x));
}


static const std::vector<STXZeroPage> STXZeroPageModeTestValues {
STXZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    STXZeroPage::Requirements{
        .initial = {
            .x = 0 },
        .final = {
            .x = 0
        }}
},
STXZeroPage{
    // One before the end of a page
    ZeroPage().address(0x00FE).zp_address(6),
    STXZeroPage::Requirements{
        .initial = {
            .x = 0xFF },
        .final = {
            .x = 0xFF
        }}
},
STXZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x00FF).zp_address(6),
    STXZeroPage::Requirements{
        .initial = {
            .x = 0x80 },
        .final = {
            .x = 0x80
        }}
},
STXZeroPage{
    ZeroPage().address(0x8000).zp_address(16),
    STXZeroPage::Requirements{
        .initial = {
            .x = 0x7F },
        .final = {
            .x = 0x7F
        }}
},
STXZeroPage{
    ZeroPage().address(0x8000).zp_address(0xFF),
    STXZeroPage::Requirements{
        .initial = {
            .x = 1 },
        .final = {
            .x = 1
        }}
}
};

TEST_P(STXZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreXZeroPageAtVariousAddresses,
                         STXZeroPageMode,
                         testing::ValuesIn(STXZeroPageModeTestValues) );
