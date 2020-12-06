#include "addressing_mode_helpers.hpp"



struct LDY_ZeroPageXIndexed_Expectations
{
    uint8_t y;
    uint8_t x;
    NZFlags flags;
};

using LDYZeroPageXIndexed     = LDY<ZeroPageXIndexed, LDY_ZeroPageXIndexed_Expectations, 4>;
using LDYZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<LDYZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LDYZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.final.y;
}

template <>
void StoreEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture      &fixture,
                                              const ZeroPageXIndexed              &address,
                                              const LDYZeroPageXIndexed::Requirements &requirements)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.x) ] = requirements.final.y;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDYZeroPageXIndexed &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDYZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDY_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDY, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDY, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDYZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.final.y));
}


static const std::vector<LDYZeroPageXIndexed> LDYZeroPageXIndexedModeTestValues {
LDYZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x0000).zp_address(6),
    LDYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 10,
            .x = 0,
            .flags = { }
        }}
},
LDYZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x00FE).zp_address(6),
    LDYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 11,
            .x = 0,
            .flags = { }
        }}
},
LDYZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x00FF).zp_address(6),
    LDYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 32,
            .x = 0,
            .flags = { }
        }}
},
LDYZeroPageXIndexed{
    // Loading a zero affects the Z flag
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    LDYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 6,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDYZeroPageXIndexed{
    // Loading a negative affects the N flag
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    LDYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 0x80,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDYZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageXIndexedAtVariousAddresses,
                         LDYZeroPageXIndexedMode,
                         testing::ValuesIn(LDYZeroPageXIndexedModeTestValues) );
