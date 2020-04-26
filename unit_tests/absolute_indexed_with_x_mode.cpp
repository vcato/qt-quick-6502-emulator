#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"
#include "instruction_checks.hpp"

using namespace testing;

struct LDA_AbsoluteXIndexed_Expectations
{
    constexpr LDA_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t x;
    NZFlags flags;
};

using LDAAbsoluteXIndexed = LDA<AbsoluteXIndexed, LDA_AbsoluteXIndexed_Expectations, 4>;

class LDAAbsoluteXIndexedMode : public InstructionExecutorTestFixture,
                                public WithParamInterface<LDAAbsoluteXIndexed>
{
public:
    void SetUp() override
    {
        const LDAAbsoluteXIndexed &param = GetParam();

        loadOpcodeIntoMemory(param.operation,
                                  AddressMode_e::AbsoluteXIndexed,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = loByteOf(param.address.absolute_address);
        fakeMemory[param.address.instruction_address + 2] = hiByteOf(param.address.absolute_address);
        fakeMemory[param.address.absolute_address + param.requirements.final.x ] = param.requirements.final.a;

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
                                 const LDA_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.final.a ));
}

void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const LDAAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count));
}
}

static const std::vector<LDAAbsoluteXIndexed> LDAAbsoluteXIndexedModeTestValues {
LDAAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0088).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 5,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Loading a zero affects the Z flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xA0,
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
LDAAbsoluteXIndexed{
    // Loading a negative affects the N flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x10,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0xFF,
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

TEST_P(LDAAbsoluteXIndexedMode, TypicalInstructionExecution)
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

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteXIndexedAtVariousAddresses,
                         LDAAbsoluteXIndexedMode,
                         testing::ValuesIn(LDAAbsoluteXIndexedModeTestValues) );
