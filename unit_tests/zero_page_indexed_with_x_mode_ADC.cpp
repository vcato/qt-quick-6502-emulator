#include "addressing_mode_helpers.hpp"



struct ADC_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZCVFlags flags;

    uint8_t   operand;
};

using ADCZeroPageXIndexed     = ADC<ZeroPageXIndexed, ADC_ZeroPageXIndexed_Expectations, 4>;
using ADCZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<ADCZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ADCZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ADCZeroPageXIndexed &instruction_param)
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
                                                            const ADCZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_ZeroPageXIndexed_Expectations &expectations)
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
                               const Instruction<AbstractInstruction_e::ADC, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.initial.operand));
}


static const std::vector<ADCZeroPageXIndexed> ADCZeroPageXIndexedModeTestValues {
ADCZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 12 },
        .final = {
            .a = 12,
            .x = 0,
            .flags = { },
            .operand = 12
        }}
},
ADCZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x80FE).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 3,
            .flags = { },
            .operand = 12 },
        .final = {
            .a = 12,
            .x = 3,
            .flags = { },
            .operand = 12
        }}
},
ADCZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x80FF).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 1,
            .x = 22,
            .flags = { },
            .operand = 31 },
        .final = {
            .a = 32,
            .x = 22,
            .flags = { },
            .operand = 31
        }}
},
ADCZeroPageXIndexed{
    // N Flag
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
ADCZeroPageXIndexed{
    // Z Flag
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
ADCZeroPageXIndexed{
    // C Flag
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // C Flag; C Flag initially set adds one to result
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x01,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // V Flag
    // 7F + 1 = 80, C = 0, V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x7F,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // V Flag
    // 7F + 1 = 81 (C initially set), C = 0, V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x7F,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x81,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // V Flag
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x81,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // V Flag
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x7F },
        .final = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // V Flag
    // 0x80 + 0x80 = 0x00 (-128 + -128 = -256), C = 1, V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
},
ADCZeroPageXIndexed{
    // V Flag
    // 0x80 + 0xFF = -129,  V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ADCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0xFF },
        .final = {
            .a = 0x7F,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
}
};

TEST_P(ADCZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddZeroPageXIndexedAtVariousAddresses,
                         ADCZeroPageXIndexedMode,
                         testing::ValuesIn(ADCZeroPageXIndexedModeTestValues) );
