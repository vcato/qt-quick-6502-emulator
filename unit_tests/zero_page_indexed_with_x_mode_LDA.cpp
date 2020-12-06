#include "addressing_mode_helpers.hpp"



struct LDA_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZFlags flags;
};

using LDAZeroPageXIndexed     = LDA<ZeroPageXIndexed, LDA_ZeroPageXIndexed_Expectations, 4>;
using LDAZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<LDAZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LDAZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.final.a;

}

template <>
void StoreEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture      &fixture,
                                              const ZeroPageXIndexed              &address,
                                              const LDAZeroPageXIndexed::Requirements &requirements)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.x) ] = requirements.final.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDAZeroPageXIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDAZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.final.a));
}


static const std::vector<LDAZeroPageXIndexed> LDAZeroPageXIndexedModeTestValues {
LDAZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x0000).zp_address(6),
    LDAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 10,
            .x = 0,
            .flags = { }
        }}
},
LDAZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x00FE).zp_address(6),
    LDAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 11,
            .x = 0,
            .flags = { }
        }}
},
LDAZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x00FF).zp_address(6),
    LDAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 32,
            .x = 0,
            .flags = { }
        }}
},
LDAZeroPageXIndexed{
    // Loading a zero affects the Z flag
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    LDAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDAZeroPageXIndexed{
    // Loading a negative affects the N flag
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    LDAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDAZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageXIndexedAtVariousAddresses,
                         LDAZeroPageXIndexedMode,
                         testing::ValuesIn(LDAZeroPageXIndexedModeTestValues) );
