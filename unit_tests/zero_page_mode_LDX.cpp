#include "addressing_mode_helpers.hpp"



struct LDX_ZeroPage_Expectations
{
    uint8_t x;
    NZFlags flags;
};

using LDXZeroPage     = LDX<ZeroPage, LDX_ZeroPage_Expectations, 3>;
using LDXZeroPageMode = ParameterizedInstructionExecutorTestFixture<LDXZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LDXZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.final.x;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDXZeroPage &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDXZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDX_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDX, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDX, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDXZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.final.x));
}


static const std::vector<LDXZeroPage> LDXZeroPageModeTestValues {
LDXZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    LDXZeroPage::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 10,
            .flags = { }
        }}
},
LDXZeroPage{
    // One before the end of a page
    ZeroPage().address(0x00FE).zp_address(6),
    LDXZeroPage::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 11,
            .flags = { }
        }}
},
LDXZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x00FF).zp_address(6),
    LDXZeroPage::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 32,
            .flags = { }
        }}
},
LDXZeroPage{
    // Loading a zero affects the Z flag
    ZeroPage().address(0x8000).zp_address(16),
    LDXZeroPage::Requirements{
        .initial = {
            .x = 6,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDXZeroPage{
    // Loading a negative affects the N flag
    ZeroPage().address(0x8000).zp_address(0xFF),
    LDXZeroPage::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDXZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageAtVariousAddresses,
                         LDXZeroPageMode,
                         testing::ValuesIn(LDXZeroPageModeTestValues) );
