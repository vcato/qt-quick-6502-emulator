#include "addressing_mode_helpers.hpp"



struct STA_ZeroPage_Expectations
{
    uint8_t a;
};

using STAZeroPage     = STA<ZeroPage, STA_ZeroPage_Expectations, 3>;
using STAZeroPageMode = ParameterizedInstructionExecutorTestFixture<STAZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAZeroPage &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.a));
}


static const std::vector<STAZeroPage> STAZeroPageModeTestValues {
STAZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    STAZeroPage::Requirements{
        .initial = {
            .a = 0 },
        .final = {
            .a = 0
        }}
},
STAZeroPage{
    // One before the end of a page
    ZeroPage().address(0x00FE).zp_address(6),
    STAZeroPage::Requirements{
        .initial = {
            .a = 0xFF },
        .final = {
            .a = 0xFF
        }}
},
STAZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x00FF).zp_address(6),
    STAZeroPage::Requirements{
        .initial = {
            .a = 0x80 },
        .final = {
            .a = 0x80
        }}
},
STAZeroPage{
    ZeroPage().address(0x8000).zp_address(16),
    STAZeroPage::Requirements{
        .initial = {
            .a = 0x7F },
        .final = {
            .a = 0x7F
        }}
},
STAZeroPage{
    ZeroPage().address(0x8000).zp_address(0xFF),
    STAZeroPage::Requirements{
        .initial = {
            .a = 1 },
        .final = {
            .a = 1
        }}
}
};

TEST_P(STAZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorZeroPageAtVariousAddresses,
                         STAZeroPageMode,
                         testing::ValuesIn(STAZeroPageModeTestValues) );
