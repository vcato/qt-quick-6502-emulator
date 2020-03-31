#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"

using namespace testing;

struct LDA_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZFlags flags;
};

using LDARequirements = Requirements<LDA_ZeroPageXIndexed_Expectations>;
using LDAZeroPageXIndexed = LDA<ZeroPageXIndexed, LDA_ZeroPageXIndexed_Expectations>;

class LDAZeroPageXIndexedMode : public InstructionExecutorTestFixture,
                                public WithParamInterface<LDAZeroPageXIndexed>
{
public:
    void SetUp() override
    {
        const LDAZeroPageXIndexed &param = GetParam();

        loadInstructionIntoMemory(param.operation,
                                  AddressMode_e::ZeroPageXIndexed,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = param.address.zero_page_address;

        // Load expected data into memory
        fakeMemory[param.address.zero_page_address + param.requirements.initial.x] = param.requirements.final.a;

        // Load appropriate registers
        r.a = param.requirements.initial.a;
        r.x = param.requirements.initial.x;
        r.SetFlag(FLAGS6502::N, param.requirements.initial.flags.n_value.expected_value);
        r.SetFlag(FLAGS6502::Z, param.requirements.initial.flags.z_value.expected_value);
    }

};

static const std::vector<LDAZeroPageXIndexed> LDAZeroPageXIndexedModeTestValues {
LDAZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x0000).zp_address(6),
    LDARequirements{
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
    LDARequirements{
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
    LDARequirements{
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
    LDARequirements{
        .initial = {
            .a = 6,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDAZeroPageXIndexed{
    // Loading a negative affects the N flag
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    LDARequirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } }
        }}
}
};

TEST_P(LDAZeroPageXIndexedMode, CheckInstructionRequirements)
{
    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq(GetParam().address.zero_page_address));
    EXPECT_THAT(fakeMemory.at( GetParam().address.zero_page_address + GetParam().requirements.initial.x), Eq(GetParam().requirements.final.a));

    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.initial.a));
    EXPECT_THAT(executor.registers().x, Eq(GetParam().requirements.initial.x));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.initial.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.initial.flags.n_value.expected_value));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.final.a));
    EXPECT_THAT(executor.registers().x, Eq(GetParam().requirements.final.x));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.final.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.final.flags.z_value.expected_value));
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageXIndexedAtVariousAddresses,
                         LDAZeroPageXIndexedMode,
                         testing::ValuesIn(LDAZeroPageXIndexedModeTestValues) );
