#include "addressing_mode_helpers.hpp"



struct AND_XIndexedIndirect_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  x;
    NZFlags  flags;

    uint8_t  operand;
};

using ANDXIndexedIndirect     = AND<XIndexedIndirect, AND_XIndexedIndirect_Expectations, 6>;
using ANDXIndexedIndirectMode = ParameterizedInstructionExecutorTestFixture<ANDXIndexedIndirect>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ANDXIndexedIndirect &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ANDXIndexedIndirect &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ANDXIndexedIndirect            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarry(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const AND_XIndexedIndirect_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::AND, XIndexedIndirect> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::AND, AddressMode_e::XIndexedIndirect) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ANDXIndexedIndirect            &instruction)
{
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t x_register    = instruction.requirements.initial.x;
    const uint8_t zero_page_address_to_load_from = fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, x_register);
    const uint8_t value         = instruction.requirements.initial.operand;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from     ), Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1 ), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page ), Eq(value));
}


static const std::vector<ANDXIndexedIndirect> ANDXIndexedIndirectModeTestValues {
ANDXIndexedIndirect{
    // Beginning of a page
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDXIndexedIndirect{
    // Middle of a page
    XIndexedIndirect().address(0x8088).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDXIndexedIndirect{
    // End of a page
    XIndexedIndirect().address(0x80FE).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDXIndexedIndirect{
    // Crossing a page
    XIndexedIndirect().address(0x80FF).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDXIndexedIndirect{
    // Check for masking out the high bit
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0xFF,
            .x = 4,
            .flags = { },
            .operand = 0x80},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0x80,
            .x = 4,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
ANDXIndexedIndirect{
    // Use alternating bits for a zero result
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0b10101010,
            .x = 0,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0x00,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0b01010101
        }}
},
ANDXIndexedIndirect{
    // Use the same bits for the same result
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA080,
            .a = 0b10101010,
            .x = 16,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .address_to_indirect_to = 0xA080,
            .a = 0b10101010,
            .x = 16,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0b10101010
        }}
},
ANDXIndexedIndirect{
    // Use the same bits for the same result (not the same pattern as before)
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    ANDXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0b01010101,
            .x = 0xFF,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0b01010101,
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
            .operand = 0b01010101
        }}
},
};

TEST_P(ANDXIndexedIndirectMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AndXIndexedIndirectAtVariousAddresses,
                         ANDXIndexedIndirectMode,
                         testing::ValuesIn(ANDXIndexedIndirectModeTestValues) );
