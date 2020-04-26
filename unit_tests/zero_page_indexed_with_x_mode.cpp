#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"
#include "instruction_checks.hpp"

using namespace testing;

struct LDA_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZFlags flags;
};

using LDAZeroPageXIndexed = LDA<ZeroPageXIndexed, LDA_ZeroPageXIndexed_Expectations, 4>;

class LDAZeroPageXIndexedMode : public InstructionExecutorTestFixture,
                                public WithParamInterface<LDAZeroPageXIndexed>
{
public:
    void SetUp() override
    {
        const LDAZeroPageXIndexed &param = GetParam();

        loadOpcodeIntoMemory(param.operation,
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

namespace
{
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.final.a));
}

void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const LDAZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count));
}
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
    LDAZeroPageXIndexed::Requirements{
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

TEST_P(LDAZeroPageXIndexedMode, TypicalInstructionExecution)
{
    // Initial expectations
    EXPECT_TRUE(ProgramCounterIsSetToInstructionAddress(executor, GetParam()));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    MemoryContainsInstruction(*this, GetParam());
    MemoryContainsExpectedComputation(*this, GetParam());
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.initial);

    executeInstruction();

    EXPECT_TRUE(ProgramCounterIsSetToOnePastTheEntireInstruction(executor, GetParam()));
    EXPECT_THAT(executor.complete(), Eq(true));
    InstructionExecutedInExpectedClockTicks(*this, GetParam());
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.final);
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageXIndexedAtVariousAddresses,
                         LDAZeroPageXIndexedMode,
                         testing::ValuesIn(LDAZeroPageXIndexedModeTestValues) );
