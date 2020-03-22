#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"

using namespace testing;

struct LDA_AbsoluteYIndexed_Expectations
{
    constexpr LDA_AbsoluteYIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t y;
    NZFlags flags;
};

using LDARequirements = Requirements<LDA_AbsoluteYIndexed_Expectations>;
using LDAAbsoluteYIndexed = LDA<AbsoluteYIndexed, LDA_AbsoluteYIndexed_Expectations>;

class LDAAbsoluteYIndexedMode : public InstructionExecutorTestFixture,
                                public WithParamInterface<LDAAbsoluteYIndexed>
{
public:
    void setup_LDA_AbsoluteYIndexed(const LDAAbsoluteYIndexed &param)
    {
        loadInstructionIntoMemory(param.operation,
                                  AddressMode_e::AbsoluteYIndexed,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = loByteOf(param.address.absolute_address);
        fakeMemory[param.address.instruction_address + 2] = hiByteOf(param.address.absolute_address);
        fakeMemory[param.address.absolute_address + param.requirements.final.y ] = param.requirements.final.a;

        // Load appropriate registers
        r.a = param.requirements.initial.a;
        r.y = param.requirements.initial.y;
        r.SetFlag(FLAGS6502::N, param.requirements.initial.flags.n_value.expected_value);
        r.SetFlag(FLAGS6502::Z, param.requirements.initial.flags.z_value.expected_value);
    }
};

static const std::vector<LDAAbsoluteYIndexed> LDAAbsoluteYIndexedModeTestValues {
LDAAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .y = 5,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 5,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Loading a zero affects the Z flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0xA0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDAAbsoluteYIndexed{
    // Loading a negative affects the N flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0x10,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .y = 0,
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

TEST_P(LDAAbsoluteYIndexedMode, CheckInstructionRequirements)
{
    const addressType &address = GetParam().address.instruction_address;
    const addressType &address_to_load_from = GetParam().address.absolute_address;
    const uint8_t     &value_to_load = GetParam().requirements.final.a;

    setup_LDA_AbsoluteYIndexed(GetParam());

    // Initial expectations
    EXPECT_THAT(executor.registers().program_counter, Eq(address));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteYIndexed) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 1), Eq( loByteOf(address_to_load_from) ));
    EXPECT_THAT(fakeMemory.at( executor.registers().program_counter + 2), Eq( hiByteOf(address_to_load_from) ));
    EXPECT_THAT(fakeMemory.at( address_to_load_from + GetParam().requirements.final.y ), Eq( value_to_load ));
    EXPECT_THAT(executor.registers().a, Eq(GetParam().requirements.initial.a));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.initial.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.initial.flags.n_value.expected_value));

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address + 3));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    EXPECT_THAT(executor.registers().a, Eq(value_to_load));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::N), Eq(GetParam().requirements.final.flags.n_value.expected_value));
    EXPECT_THAT(executor.registers().GetFlag(FLAGS6502::Z), Eq(GetParam().requirements.final.flags.z_value.expected_value));
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteYIndexedAtVariousAddresses,
                         LDAAbsoluteYIndexedMode,
                         testing::ValuesIn(LDAAbsoluteYIndexedModeTestValues) );
