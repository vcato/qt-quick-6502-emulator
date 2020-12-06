#include "addressing_mode_helpers.hpp"



struct ADC_XIndexedIndirect_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  x;
    NZCVFlags flags;

    uint8_t   addend;
};

using ADCXIndexedIndirect     = ADC<XIndexedIndirect, ADC_XIndexedIndirect_Expectations, 6>;
using ADCXIndexedIndirectMode = ParameterizedInstructionExecutorTestFixture<ADCXIndexedIndirect>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ADCXIndexedIndirect &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to] = instruction_param.requirements.initial.addend;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ADCXIndexedIndirect &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ADCXIndexedIndirect            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarry(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_XIndexedIndirect_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ADC, XIndexedIndirect> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::XIndexedIndirect) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCXIndexedIndirect            &instruction)
{
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t x_register    = instruction.requirements.initial.x;
    const uint8_t zero_page_address_to_load_from = fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, x_register);
    const uint8_t value         = instruction.requirements.initial.addend;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from ),    Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page ), Eq(value));
}


static const std::vector<ADCXIndexedIndirect> ADCXIndexedIndirectModeTestValues {
ADCXIndexedIndirect{
    // Beginning of a page
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = { },
            .addend = 6},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 2
        }}
},
ADCXIndexedIndirect{
    // End of a page
    XIndexedIndirect().address(0x80FE).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = { },
            .addend = 6},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 2
        }}
},
ADCXIndexedIndirect{
    // Crossing a page
    XIndexedIndirect().address(0x80FF).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = { },
            .addend = 6},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 2
        }}
},
ADCXIndexedIndirect{
    // Adding a zero affects the Z flag
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = { },
            .addend = 0},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0
        }}
},
ADCXIndexedIndirect{
    // Adding a negative affects the N flag
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = { },
            .addend = 0x80},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x80
        }}
},
// Carry flag
ADCXIndexedIndirect{
    // 1 + 1 = 2, C = 0, V=0
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .x = 12,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 2,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x80
        }}
},
ADCXIndexedIndirect{
    // FF + 1 = 0, C = 1, V = 0
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .x = 12,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 1
        }}
},
ADCXIndexedIndirect{
    // 1 + FF = 0, C = 1, V = 0
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .x = 12,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCXIndexedIndirect{
    // 7F + 1 = 80, C = 0, V = 1
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .x = 12,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .addend = 1
        }}
},
ADCXIndexedIndirect{
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x81,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 1
        }}
},
ADCXIndexedIndirect{
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12,
            .flags = { },
            .addend = 0x7F},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x7F
        }}
},
ADCXIndexedIndirect{
    // 0x80 + 0x80 = 0x00 (-128 + -127 = -256), C = 1, V = 1
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12,
            .flags = { },
            .addend = 0x80},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0x7F
        }}
},
ADCXIndexedIndirect{
    // 0x80 + 0xFF = -129,  V = 1
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0x7F
        }}
},
ADCXIndexedIndirect{
    // 0x00 + 0x02 + C = 0x03,
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 2},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 3,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x7F
        }}
},
ADCXIndexedIndirect{
    // 0xFF + 0x01 + C = 0x01,
    XIndexedIndirect().address(0x8080).zp_address(0xA0),
    ADCXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .x = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 1
        }}
}
};

TEST_P(ADCXIndexedIndirectMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddXIndexedIndirectAtVariousAddresses,
                         ADCXIndexedIndirectMode,
                         testing::ValuesIn(ADCXIndexedIndirectModeTestValues) );
