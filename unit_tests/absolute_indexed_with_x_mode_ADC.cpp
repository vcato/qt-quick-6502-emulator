#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ADC_AbsoluteXIndexed_Expectations
{
    constexpr ADC_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t x;
    NZCVFlags flags;

    uint8_t   addend;
};

using ADCAbsoluteXIndexed     = ADC<AbsoluteXIndexed, ADC_AbsoluteXIndexed_Expectations, 4>;
using ADCAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<ADCAbsoluteXIndexed>;


static void StoreOperand(InstructionExecutorTestFixture &fixture, const ADCAbsoluteXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
}

static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ADCAbsoluteXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.initial.addend;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ADCAbsoluteXIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

static void SetupRAMForInstructionsThatHaveAnEffectiveAddress(InstructionExecutorTestFixture &fixture,
                                                              const ADCAbsoluteXIndexed      &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreOperand(fixture, instruction_param);
    StoreTestValueAtEffectiveAddress(fixture, instruction_param);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(InstructionExecutorTestFixture &fixture,
                                                            const ADCAbsoluteXIndexed      &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ADC, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.initial.addend ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const ADCAbsoluteXIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.x;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<ADCAbsoluteXIndexed> ADCAbsoluteXIndexedModeTestValues {
ADCAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0088).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .x = 5,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteXIndexed{
    // Adding a zero affects the Z flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .addend = 0},
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 6
        }}
},
ADCAbsoluteXIndexed{
    // Adding a negative affects the N flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0,
            .flags = { },
            .addend = 0x80},
        .final = {
            .a = 0x80,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x80
        }}
},
// Carry flag
ADCAbsoluteXIndexed{
    // 1 + 1 = 2, C = 0, V=0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .x = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x02,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsoluteXIndexed{
    // FF + 1 = 0, C = 1, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x00,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsoluteXIndexed{
    // 1 + FF = 0, C = 1, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .x = 0x80,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .a = 0x00,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCAbsoluteXIndexed{
    // 7F + 1 = 80, C = 0, V = 1
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x7f,
            .x = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x80,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .addend = 0x01
        }}
},
ADCAbsoluteXIndexed{
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x81,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsoluteXIndexed{
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 0x80,
            .flags = { },
            .addend = 0x7F},
        .final = {
            .a = 0xFF,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x7F
        }}
},
ADCAbsoluteXIndexed{
    // 0x80 + 0x80 = 0x00 (-128 + -127 = -256), C = 1, V = 1
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 0x80,
            .flags = { },
            .addend = 0x80},
        .final = {
            .a = 0x00,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0x80
        }}
},
ADCAbsoluteXIndexed{
    // 0x80 + 0xFF = -129,  V = 1
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 0x80,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .a = 0x7F,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0xFF
        }}
},
ADCAbsoluteXIndexed{
    // 0x00 + 0x02 + C = 0x03,
    AbsoluteXIndexed().address(0x8000).value(0x02),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x02},
        .final = {
            .a = 0x03,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCAbsoluteXIndexed{
    // 0xFF + 0x01 + C = 0x01,
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01},
        .final = {
            .a = 0x01,
            .x = 0x80,
            .flags = {
                .n_value = {
                    .expected_value = false },
                .z_value = {
                    .expected_value = false },
                .c_value = {
                    .expected_value = true }, // Carry should be set because we wrapped around
                .v_value = {
                    .expected_value = false } },
            .addend = 0x01
        }}
}
};

TEST_P(ADCAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddAbsoluteXIndexedAtVariousAddresses,
                         ADCAbsoluteXIndexedMode,
                         testing::ValuesIn(ADCAbsoluteXIndexedModeTestValues) );
