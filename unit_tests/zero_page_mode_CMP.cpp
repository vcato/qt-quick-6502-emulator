#include "addressing_mode_helpers.hpp"



struct CMP_ZeroPage_Expectations
{
    uint8_t  a;
    NZCFlags flags;

    uint8_t  operand;
};

using CMPZeroPage     = CMP<ZeroPage, CMP_ZeroPage_Expectations, 3>;
using CMPZeroPageMode = ParameterizedInstructionExecutorTestFixture<CMPZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const CMPZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CMPZeroPage &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CMPZeroPage                    &)
{
    // No Memory Affected!
}


static const std::vector<CMPZeroPage> CMPZeroPageModeTestValues {
CMPZeroPage{
    // Beginning of a page
    // 0 - 0
    ZeroPage().address(0xA000).zp_address(6),
    CMPZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPZeroPage{
    // Middle of a page
    // 0 - 0
    ZeroPage().address(0xA080).zp_address(0x80),
    CMPZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPZeroPage{
    // End of a page
    // 0xFF - 0
    ZeroPage().address(0xA0FE).zp_address(0xFF),
    CMPZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPZeroPage{
    // Crossing a page
    // 0 - 0xFF
    ZeroPage().address(0xA0FF).zp_address(0xCF),
    CMPZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
CMPZeroPage{
    // 2 - 3
    // Negative Result
    ZeroPage().address(0x8000).zp_address(0xA0),
    CMPZeroPage::Requirements{
        .initial = {
            .a = 0x02,
            .flags = { },
            .operand = 0x03 },
        .final = {
            .a = 0x02,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0x03
        }}
},
CMPZeroPage{
    // 3 - 2
    // Positive Result
    ZeroPage().address(0x8000).zp_address(0xA0),
    CMPZeroPage::Requirements{
        .initial = {
            .a = 0x03,
            .flags = { },
            .operand = 0x02 },
        .final = {
            .a = 0x03,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0x02
        }}
}
};

TEST_P(CMPZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorZeroPageAtVariousAddresses,
                         CMPZeroPageMode,
                         testing::ValuesIn(CMPZeroPageModeTestValues) );
