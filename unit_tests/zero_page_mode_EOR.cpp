#include "addressing_mode_helpers.hpp"



struct EOR_ZeroPage_Expectations
{
    uint8_t a;
    NZFlags flags;

    uint8_t operand;
};

using EORZeroPage     = EOR<ZeroPage, EOR_ZeroPage_Expectations, 3>;
using EORZeroPageMode = ParameterizedInstructionExecutorTestFixture<EORZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const EORZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const EORZeroPage &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const EORZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const EOR_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::EOR, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::EOR, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const EORZeroPage                    &)
{
    // No memory affected
}


static const std::vector<EORZeroPage> EORZeroPageModeTestValues {
EORZeroPage{
    // Beginning of a page
    ZeroPage().address(0x1000).zp_address(0xA0),
    EORZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
EORZeroPage{
    // One before the end of a page
    ZeroPage().address(0x10FE).zp_address(0xA0),
    EORZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
EORZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x10FF).zp_address(0xA0),
    EORZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
EORZeroPage{
    // Test individual bits are EOR'd.
    ZeroPage().address(0x8000).zp_address(0x0F),
    EORZeroPage::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0b10101010
        }}
},
EORZeroPage{
    // Test individual bits are EOR'd.
    ZeroPage().address(0x8000).zp_address(0xFF),
    EORZeroPage::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0b01010101
        }}
},
EORZeroPage{
    // Test individual bits are EOR'd.
    ZeroPage().address(0x8000).zp_address(0xFF),
    EORZeroPage::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b11111111,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(EORZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ExclusiveOrZeroPageAtVariousAddresses,
                         EORZeroPageMode,
                         testing::ValuesIn(EORZeroPageModeTestValues) );
