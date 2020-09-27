#include "addressing_mode_helpers.hpp"



struct LDA_ZeroPage_Expectations
{
    uint8_t a;
    NZFlags flags;
};

using LDAZeroPage     = LDA<ZeroPage, LDA_ZeroPage_Expectations, 3>;
using LDAZeroPageMode = ParameterizedInstructionExecutorTestFixture<LDAZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LDAZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.final.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDAZeroPage &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDAZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.final.a));
}


static const std::vector<LDAZeroPage> LDAZeroPageModeTestValues {
LDAZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    LDAZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 10,
            .flags = { }
        }}
},
LDAZeroPage{
    // One before the end of a page
    ZeroPage().address(0x00FE).zp_address(6),
    LDAZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 11,
            .flags = { }
        }}
},
LDAZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x00FF).zp_address(6),
    LDAZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 32,
            .flags = { }
        }}
},
LDAZeroPage{
    // Loading a zero affects the Z flag
    ZeroPage().address(0x8000).zp_address(16),
    LDAZeroPage::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDAZeroPage{
    // Loading a negative affects the N flag
    ZeroPage().address(0x8000).zp_address(0xFF),
    LDAZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDAZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageAtVariousAddresses,
                         LDAZeroPageMode,
                         testing::ValuesIn(LDAZeroPageModeTestValues) );
