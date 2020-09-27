#include "addressing_mode_helpers.hpp"



struct CMP_XIndexedIndirect_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  x;
    NZCFlags flags;

    uint8_t  operand;
};

using CMPXIndexedIndirect     = CMP<XIndexedIndirect, CMP_XIndexedIndirect_Expectations, 6>;
using CMPXIndexedIndirectMode = ParameterizedInstructionExecutorTestFixture<CMPXIndexedIndirect>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const CMPXIndexedIndirect &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CMPXIndexedIndirect &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPXIndexedIndirect            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarry(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_XIndexedIndirect_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, XIndexedIndirect> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::XIndexedIndirect) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const CMPXIndexedIndirect            &instruction)
{
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t x_register    = instruction.requirements.initial.x;
    const uint8_t zero_page_address_to_load_from = fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, x_register);
    const uint8_t value         = instruction.requirements.initial.operand;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from     ), Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1 ), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page ), Eq(value));
}


static const std::vector<CMPXIndexedIndirect> CMPXIndexedIndirectModeTestValues {
CMPXIndexedIndirect{
    // Beginning of a page
    // 0 - 0
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    CMPXIndexedIndirect::Requirements{
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
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
CMPXIndexedIndirect{
    // Middle of a page
    // 0 - 0
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    CMPXIndexedIndirect::Requirements{
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
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
CMPXIndexedIndirect{
    // End of a page
    // 0 - 0
    XIndexedIndirect().address(0x80FE).zp_address(0xA0),
    CMPXIndexedIndirect::Requirements{
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
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
CMPXIndexedIndirect{
    // Crossing a page
    XIndexedIndirect().address(0x80FF).zp_address(0xA0),
    CMPXIndexedIndirect::Requirements{
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
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
CMPXIndexedIndirect{
    // 2 - 3
    // Negative Result
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    CMPXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 2,
            .x = 0x10,
            .flags = { },
            .operand = 3 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 2,
            .x = 0x10,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 3
        }}
},
CMPXIndexedIndirect{
    // 3 - 2
    // Positive Result
    XIndexedIndirect().address(0x8000).zp_address(0xFF),
    CMPXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA0FF,
            .a = 3,
            .x = 0x10,
            .flags = { },
            .operand = 2 },
        .final = {
            .address_to_indirect_to = 0xA0FF,
            .a = 3,
            .x = 0x10,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 2
        }}
}
};

TEST_P(CMPXIndexedIndirectMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorXIndexedIndirectAtVariousAddresses,
                         CMPXIndexedIndirectMode,
                         testing::ValuesIn(CMPXIndexedIndirectModeTestValues) );
