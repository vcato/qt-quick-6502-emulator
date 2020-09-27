#include "addressing_mode_helpers.hpp"



struct LDY_ZeroPage_Expectations
{
    uint8_t y;
    NZFlags flags;
};

using LDYZeroPage     = LDY<ZeroPage, LDY_ZeroPage_Expectations, 3>;
using LDYZeroPageMode = ParameterizedInstructionExecutorTestFixture<LDYZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LDYZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.final.y;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDYZeroPage &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDYZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDY_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDY, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDY, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDYZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.final.y));
}


static const std::vector<LDYZeroPage> LDYZeroPageModeTestValues {
LDYZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    LDYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 10,
            .flags = { }
        }}
},
LDYZeroPage{
    // One before the end of a page
    ZeroPage().address(0x00FE).zp_address(6),
    LDYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 11,
            .flags = { }
        }}
},
LDYZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x00FF).zp_address(6),
    LDYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 32,
            .flags = { }
        }}
},
LDYZeroPage{
    // Loading a zero affects the Z flag
    ZeroPage().address(0x8000).zp_address(16),
    LDYZeroPage::Requirements{
        .initial = {
            .y = 6,
            .flags = { }},
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDYZeroPage{
    // Loading a negative affects the N flag
    ZeroPage().address(0x8000).zp_address(0xFF),
    LDYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDYZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageAtVariousAddresses,
                         LDYZeroPageMode,
                         testing::ValuesIn(LDYZeroPageModeTestValues) );
