#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include "instruction_definitions.hpp"
#include "instruction_checks.hpp"

using namespace testing;

struct LDA_XIndexedIndirect_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  x;
    NZFlags  flags;
};

using LDAXIndexedIndirect = LDA<XIndexedIndirect, LDA_XIndexedIndirect_Expectations, 6>;

class LDAXIndexedIndirectMode : public InstructionExecutorTestFixture,
                                public WithParamInterface<LDAXIndexedIndirect>
{
public:
    void SetUp() override
    {
        const LDAXIndexedIndirect &param = GetParam();

        loadOpcodeIntoMemory(param.operation,
                                  AddressMode_e::XIndexedIndirect,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = param.address.zero_page_address;
        fakeMemory[param.address.zero_page_address + param.requirements.initial.x    ] = loByteOf(param.requirements.initial.address_to_indirect_to);
        fakeMemory[param.address.zero_page_address + param.requirements.initial.x + 1] = hiByteOf(param.requirements.initial.address_to_indirect_to);
        fakeMemory[param.requirements.initial.address_to_indirect_to] = param.requirements.final.a;

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
                                 const LDA_XIndexedIndirect_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, XIndexedIndirect> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::XIndexedIndirect) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAXIndexedIndirect            &instruction)
{
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t zero_page_address_to_load_from = instruction.address.zero_page_address;
    const uint8_t value_to_load = instruction.requirements.final.a;
    const uint8_t x_register    = instruction.requirements.initial.x;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + x_register),     Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + x_register + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page ), Eq(value_to_load));
}

void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const LDAXIndexedIndirect            &instruction)
{
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count));
}

void SetupTypicalExecutionState(const InstructionExecutorTestFixture &fixture,
                                const LDAXIndexedIndirect            &instruction)
{
    EXPECT_TRUE(ProgramCounterIsSetToInstructionAddress(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(0U));
    MemoryContainsInstruction(fixture, instruction);
    MemoryContainsExpectedComputation(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.initial);
}

void CheckTypicalExecutionResults(const InstructionExecutorTestFixture &fixture,
                                  const LDAXIndexedIndirect            &instruction)
{
    EXPECT_TRUE(ProgramCounterIsSetToOnePastTheEntireInstruction(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    InstructionExecutedInExpectedClockTicks(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.final);
}
}

static const std::vector<LDAXIndexedIndirect> LDAXIndexedIndirectModeTestValues {
LDAXIndexedIndirect{
    // Beginning of a page
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    LDAXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = { }},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .x = 12,
            .flags = { }
        }}
}
};

TEST_P(LDAXIndexedIndirectMode, TypicalInstructionExecution)
{
    SetupTypicalExecutionState(*this, GetParam());

    executeInstruction();

    CheckTypicalExecutionResults(*this, GetParam());
}

INSTANTIATE_TEST_CASE_P(LoadXIndexedIndirectAtVariousAddresses,
                         LDAXIndexedIndirectMode,
                         testing::ValuesIn(LDAXIndexedIndirectModeTestValues) );
