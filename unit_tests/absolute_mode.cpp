#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"
#include "instruction_checks.hpp"

using namespace testing;

struct LDA_Absolute_Expectations
{
    constexpr LDA_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;
};

using LDARequirements = Requirements<LDA_Absolute_Expectations>;
using LDAAbsolute = LDA<Absolute, LDA_Absolute_Expectations>;

class LDAAbsoluteMode : public InstructionExecutorTestFixture,
                        public WithParamInterface<LDAAbsolute>
{
public:
    void SetUp() override
    {
        const LDAAbsolute &param = GetParam();

        loadOpcodeIntoMemory(param.operation,
                                  AddressMode_e::Absolute,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = loByteOf(param.address.absolute_address);
        fakeMemory[param.address.instruction_address + 2] = hiByteOf(param.address.absolute_address);
        fakeMemory[param.address.absolute_address       ] = param.requirements.final.a;

        // Load appropriate registers
        r.a = param.requirements.initial.a;
        r.SetFlag(FLAGS6502::N, param.requirements.initial.flags.n_value.expected_value);
        r.SetFlag(FLAGS6502::Z, param.requirements.initial.flags.z_value.expected_value);
    }
};

namespace
{
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.final.a ));
}
}

static const std::vector<LDAAbsolute> LDAAbsoluteModeTestValues {
LDAAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Loading a zero affects the Z flag
    Absolute().address(0x8000).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0xA0,
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
LDAAbsolute{
    // Loading a negative affects the N flag
    Absolute().address(0x8000).value(0xA000),
    LDARequirements{
        .initial = {
            .a = 0x10,
            .flags = { }},
        .final = {
            .a = 0xFF,
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

TEST_P(LDAAbsoluteMode, CheckInstructionRequirements)
{
    // Initial expectations
    EXPECT_TRUE(ProgramCounterIsSetToInstructionAddress(executor, GetParam()));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    MemoryContainsInstruction(*this, GetParam());
    MemoryContainsExpectedComputation(*this, GetParam());
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.initial);

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address + GetParam().address.operand_byte_count + 1));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.final);
}

INSTANTIATE_TEST_CASE_P(LoadAbsoluteAtVariousAddresses,
                         LDAAbsoluteMode,
                         testing::ValuesIn(LDAAbsoluteModeTestValues) );
