#include "addressing_mode_helpers.hpp"



struct ADC_ZeroPage_Expectations
{
    uint8_t   a;
    NZCVFlags flags;

    uint8_t   addend;
};

using ADCZeroPage     = ADC<ZeroPage, ADC_ZeroPage_Expectations, 3>;
using ADCZeroPageMode = ParameterizedInstructionExecutorTestFixture<ADCZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ADCZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.addend;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ADCZeroPage &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ADCZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ADC, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.addend));
}


static const std::vector<ADCZeroPage> ADCZeroPageModeTestValues {
ADCZeroPage{
    // Beginning of a page
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 12 },
        .final = {
            .a = 12,
            .flags = { },
            .addend = 12
        }}
},
ADCZeroPage{
    // One before the end of a page
    ZeroPage().address(0x80FE).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 12 },
        .final = {
            .a = 12,
            .flags = { },
            .addend = 12
        }}
},
ADCZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x80FF).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 12 },
        .final = {
            .a = 12,
            .flags = { },
            .addend = 12
        }}
},
ADCZeroPage{
    // N Flag
    ZeroPage().address(0x8080).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x80 },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x80
        }}
},
ADCZeroPage{
    // Z Flag
    ZeroPage().address(0x8080).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0
        }}
},
ADCZeroPage{
    // C Flag
    ZeroPage().address(0x8080).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01 },
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // C Flag; C Flag initially set adds one to result
    ZeroPage().address(0x8080).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01 },
        .final = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // V Flag
    // 7F + 1 = 80, C = 0, V = 1
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01 },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // V Flag
    // 7F + 1 = 81 (C initially set), C = 0, V = 1
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01 },
        .final = {
            .a = 0x81,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // V Flag
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01 },
        .final = {
            .a = 0x81,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // V Flag
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x7F },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // V Flag
    // 0x80 + 0x80 = 0x00 (-128 + -128 = -256), C = 1, V = 1
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x80 },
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0x01
        }}
},
ADCZeroPage{
    // V Flag
    // 0x80 + 0xFF = -129,  V = 1
    ZeroPage().address(0x8000).zp_address(6),
    ADCZeroPage::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0xFF },
        .final = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0x01
        }}
}
};

TEST_P(ADCZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddZeroPageAtVariousAddresses,
                         ADCZeroPageMode,
                         testing::ValuesIn(ADCZeroPageModeTestValues) );
