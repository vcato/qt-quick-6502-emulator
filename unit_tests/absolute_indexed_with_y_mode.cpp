#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"
#include "instruction_checks.hpp"

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
    void SetUp() override
    {
        const LDAAbsoluteYIndexed &param = GetParam();

        loadOpcodeIntoMemory(param.operation,
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

void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

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
    //const addressType &address = GetParam().address.instruction_address;
    const addressType &address_to_load_from = GetParam().address.absolute_address;
    const uint8_t     &value_to_load = GetParam().requirements.final.a;

    // Initial expectations
    EXPECT_TRUE(ProgramCounterIsSetToInstructionAddress(executor, GetParam()));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    MemoryContainsInstruction(*this, GetParam());
    EXPECT_THAT(fakeMemory.at( address_to_load_from + GetParam().requirements.final.y ), Eq( value_to_load ));
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.initial);

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(GetParam().address.instruction_address + GetParam().address.operand_byte_count + 1));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(4U));
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.final);
}

INSTANTIATE_TEST_CASE_P(LoadAbsoluteYIndexedAtVariousAddresses,
                         LDAAbsoluteYIndexedMode,
                         testing::ValuesIn(LDAAbsoluteYIndexedModeTestValues) );
