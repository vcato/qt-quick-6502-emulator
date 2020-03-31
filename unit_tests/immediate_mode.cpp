#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"

using namespace testing;

struct LDA_Immediate_Expectations
{
    constexpr LDA_Immediate_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;
};

using LDARequirements = Requirements<LDA_Immediate_Expectations>;
using LDAImmediate = LDA<Immediate, LDA_Immediate_Expectations>;

class LDAImmediateMode : public InstructionExecutorTestFixture,
                         public WithParamInterface<LDAImmediate>
{
public:
    void SetUp() override
    {
        const LDAImmediate &param = GetParam();

        loadInstructionIntoMemory(param.operation,
                                  AddressMode_e::Immediate,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = param.address.immediate_value;

        // Load appropriate registers
        r.a = param.requirements.initial.a;
        r.SetFlag(FLAGS6502::N, param.requirements.initial.flags.n_value.expected_value);
        r.SetFlag(FLAGS6502::Z, param.requirements.initial.flags.z_value.expected_value);
    }

};

static const std::vector<LDAImmediate> LDAImmediateModeTestValues {
LDAImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(6),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(6),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(6),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAImmediate{
    // Loading a zero affects the Z flag
    Immediate().address(0x8000).value(0),
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
LDAImmediate{
    // Loading a negative affects the N flag
    Immediate().address(0x8000).value(0x80),
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

TEST_P(LDAImmediateMode, CheckInstructionRequirements)
{
    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::Immediate) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq(GetParam().address.immediate_value));
    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.initial.a));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.initial.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.initial.flags.n_value.expected_value));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address + 2));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(2U));
    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.final.a));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.final.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.final.flags.z_value.expected_value));
}

INSTANTIATE_TEST_SUITE_P(LoadImmediateAtVariousAddresses,
                         LDAImmediateMode,
                         testing::ValuesIn(LDAImmediateModeTestValues) );

