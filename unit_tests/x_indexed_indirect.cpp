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

using LDARequirements = Requirements<LDA_XIndexedIndirect_Expectations>;
using LDAXIndexedIndirect = LDA<XIndexedIndirect, LDA_XIndexedIndirect_Expectations>;

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

static const std::vector<LDAXIndexedIndirect> LDAXIndexedIndirectModeTestValues {
LDAXIndexedIndirect{
    // Beginning of a page
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    LDARequirements{
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

TEST_P(LDAXIndexedIndirectMode, CheckInstructionRequirements)
{
    const addressType address = GetParam().address.instruction_address;
    const addressType address_stored_in_zero_page    = GetParam().requirements.initial.address_to_indirect_to;
    const uint8_t     zero_page_address_to_load_from = GetParam().address.zero_page_address;
    const uint8_t     value_to_load = GetParam().requirements.final.a;
    const uint8_t     x_register    = GetParam().requirements.initial.x;

    // Initial expectations
    EXPECT_TRUE(ProgramCounterIsSetToInstructionAddress(executor, GetParam()));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
    MemoryContainsInstruction(*this, GetParam());
    EXPECT_THAT(fakeMemory.at( zero_page_address_to_load_from + x_register),     Eq( loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fakeMemory.at( zero_page_address_to_load_from + x_register + 1), Eq( hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fakeMemory.at( address_stored_in_zero_page ), Eq(value_to_load));
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.initial);

    executeInstruction();

    EXPECT_THAT(executor.registers().program_counter, Eq(address + GetParam().address.operand_byte_count + 1));
    EXPECT_THAT(executor.complete(), Eq(true));
    EXPECT_THAT(executor.clock_ticks, Eq(6U));
    RegistersAreInExpectedState(executor.registers(), GetParam().requirements.final);
}

INSTANTIATE_TEST_CASE_P(LoadXIndexedIndirectAtVariousAddresses,
                         LDAXIndexedIndirectMode,
                         testing::ValuesIn(LDAXIndexedIndirectModeTestValues) );
