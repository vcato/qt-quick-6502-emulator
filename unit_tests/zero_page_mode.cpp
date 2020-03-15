#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_helpers.hpp"

using namespace testing;

struct LDA_ZeroPage_Expectations
{
    uint8_t a;
    NZFlags flags;
};

struct LDA_ZeroPage_Requirements
{
    LDA_ZeroPage_Expectations initial;
    LDA_ZeroPage_Expectations final;
};

using LDARequirements = LDA_ZeroPage_Requirements;
using LDAZeroPage = LDA<ZeroPage, LDARequirements>;

class LDAZeroPageMode : public InstructionExecutorTestFixture,
                        public WithParamInterface<LDAZeroPage>
{
public:
    void setup_LDA_ZeroPage(const LDAZeroPage &param)
    {
        loadInstructionIntoMemory(param.operation,
                                  AddressMode_e::ZeroPage,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = param.address.zero_page_address;

        // Load expected data into memory
        fakeMemory[param.address.zero_page_address] = param.requirements.final.a;

        // Load appropriate registers
        r.a = param.requirements.initial.a;
        r.SetFlag(FLAGS6502::N, param.requirements.initial.flags.n_value.expected_value);
        r.SetFlag(FLAGS6502::Z, param.requirements.initial.flags.z_value.expected_value);
    }

};

static const std::vector<LDAZeroPage> LDAZeroPageModeTestValues {
LDAZeroPage{
    // Beginning of a page
    ZeroPage().address(0x0000).zp_address(6),
    LDARequirements{
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
    LDARequirements{
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
    LDARequirements{
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
    LDARequirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDAZeroPage{
    // Loading a negative affects the N flag
    ZeroPage().address(0x8000).zp_address(0xFF),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
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

TEST_P(LDAZeroPageMode, CheckInstructionRequirements)
{
    setup_LDA_ZeroPage(GetParam());

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq(GetParam().address.zero_page_address));
    EXPECT_THAT(fakeMemory.at( GetParam().address.zero_page_address ), Eq(GetParam().requirements.final.a));
    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.initial.a));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.initial.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.initial.flags.n_value.expected_value));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(3U));
    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.final.a));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.final.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.final.flags.z_value.expected_value));
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageAtVariousAddresses,
                         LDAZeroPageMode,
                         testing::ValuesIn(LDAZeroPageModeTestValues) );
