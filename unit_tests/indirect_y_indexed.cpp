#include <gmock/gmock.h>
#include "instruction_checks.hpp"

using namespace testing;

struct LDA_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
    NZFlags  flags;
};

using LDAIndirectYIndexed = LDA<IndirectYIndexed, LDA_IndirectYIndexed_Expectations, 5>;

class LDAIndirectYIndexedMode : public InstructionExecutorTestFixture,
                                public WithParamInterface<LDAIndirectYIndexed>
{
public:
    void SetUp() override
    {
        const LDAIndirectYIndexed &param = GetParam();

        loadOpcodeIntoMemory(param.operation,
                                  AddressMode_e::IndirectYIndexed,
                                  param.address.instruction_address);
        fakeMemory[param.address.instruction_address + 1] = param.address.zero_page_address;
        fakeMemory[param.address.zero_page_address    ] = loByteOf(param.requirements.initial.address_to_indirect_to);
        fakeMemory[param.address.zero_page_address + 1] = hiByteOf(param.requirements.initial.address_to_indirect_to);
        fakeMemory[param.requirements.initial.address_to_indirect_to + param.requirements.initial.y ] = param.requirements.final.a;

        // Load appropriate registers
        r.a = param.requirements.initial.a;
        r.y = param.requirements.initial.y;
        r.SetFlag(FLAGS6502::N, param.requirements.initial.flags.n_value.expected_value);
        r.SetFlag(FLAGS6502::Z, param.requirements.initial.flags.z_value.expected_value);
    }
};

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_IndirectYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAIndirectYIndexed            &instruction)
{
    const uint8_t zero_page_address_to_load_from = instruction.address.zero_page_address;
    const uint8_t value_to_load                  = instruction.requirements.final.a;
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t y_register                     = instruction.requirements.initial.y;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from ),    Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page + y_register ), Eq(value_to_load));
}


static const std::vector<LDAIndirectYIndexed> LDAIndirectYIndexedModeTestValues {
LDAIndirectYIndexed{
    // Beginning of a page
    IndirectYIndexed().address(0x0000).zp_address(0xA0),
    LDAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12,
            .flags = { }},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = { }
        }}
}
};

TEST_P(LDAIndirectYIndexedMode, TypicalInstructionExecution)
{
    SetupTypicalExecutionState(*this, GetParam());

    executeInstruction();

    CheckTypicalExecutionResults(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadIndirectYIndexedAtVariousAddresses,
                         LDAIndirectYIndexedMode,
                         testing::ValuesIn(LDAIndirectYIndexedModeTestValues) );
