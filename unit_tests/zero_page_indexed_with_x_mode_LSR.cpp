#include "addressing_mode_helpers.hpp"



struct LSR_ZeroPageXIndexed_Expectations
{
    uint8_t x;
    NZCFlags flags;

    uint8_t  operand;
};

using LSRZeroPageXIndexed     = LSR<ZeroPageXIndexed, LSR_ZeroPageXIndexed_Expectations, 6>;
using LSRZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<LSRZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LSRZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LSRZeroPageXIndexed &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LSRZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LSR_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LSR, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LSR, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LSRZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.initial.operand));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const LSRZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.final.operand));
}


static const std::vector<LSRZeroPageXIndexed> LSRZeroPageXIndexedModeTestValues {
LSRZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRZeroPageXIndexed{
    // Middle of a page
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRZeroPageXIndexed{
    // End of a page
    ZeroPageXIndexed().address(0x80FE).zp_address(6),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x80FF).zp_address(6),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRZeroPageXIndexed{
    // Check for Low bit going into carry
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b00101010
        }}
},
LSRZeroPageXIndexed{
    // Zero is set in highest bit
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 1,
            .flags = { },
            .operand = 0b11111111 },
        .final = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01111111
        }}
},
LSRZeroPageXIndexed{
    // Indexing the maximum stays on this page
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    LSRZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = { },
            .operand = 0b11111111 },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01111111
        }}
}
};

TEST_P(LSRZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LogicalShiftRightZeroPageXIndexedAtVariousAddresses,
                         LSRZeroPageXIndexedMode,
                         testing::ValuesIn(LSRZeroPageXIndexedModeTestValues) );
