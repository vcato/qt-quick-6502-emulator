#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ADC_AbsoluteYIndexed_Expectations
{
    constexpr ADC_AbsoluteYIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t y;
    NZCVFlags flags;

    uint8_t   addend;
};

using ADCAbsoluteYIndexed     = ADC<AbsoluteYIndexed, ADC_AbsoluteYIndexed_Expectations, 4>;
using ADCAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<ADCAbsoluteYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ADCAbsoluteYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.y ] = instruction_param.requirements.initial.addend;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ADC, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::AbsoluteYIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.y ), Eq( instruction.requirements.initial.addend ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const ADCAbsoluteYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<ADCAbsoluteYIndexed> ADCAbsoluteXIndexedModeTestValues {
ADCAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 5,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .y = 5,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsoluteYIndexed{
    // Adding a zero affects the Z flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .addend = 0},
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 6
        }}
},
ADCAbsoluteYIndexed{
    // Adding a negative affects the N flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .y = 0,
            .flags = { },
            .addend = 0x80},
        .final = {
            .a = 0x80,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x80
        }}
},
// Carry flag
ADCAbsoluteYIndexed{
    // 1 + 1 = 2, C = 0, V=0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .y = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x02,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsoluteYIndexed{
    // FF + 1 = 0, C = 1, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x00,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsoluteYIndexed{
    // 1 + FF = 0, C = 1, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .y = 0x80,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .a = 0x00,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCAbsoluteYIndexed{
    // 7F + 1 = 80, C = 0, V = 1
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x7f,
            .y = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x80,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = true } },
            .addend = 0x01
        }}
},
ADCAbsoluteYIndexed{
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 0x80,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x81,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsoluteYIndexed{
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 0x80,
            .flags = { },
            .addend = 0x7F},
        .final = {
            .a = 0xFF,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x7F
        }}
},
ADCAbsoluteYIndexed{
    // 0x80 + 0x80 = 0x00 (-128 + -127 = -256), C = 1, V = 1
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 0x80,
            .flags = { },
            .addend = 0x80},
        .final = {
            .a = 0x00,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = true } },
            .addend = 0x80
        }}
},
ADCAbsoluteYIndexed{
    // 0x80 + 0xFF = -129,  V = 1
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 0x80,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .a = 0x7F,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = true } },
            .addend = 0xFF
        }}
},
ADCAbsoluteYIndexed{
    // 0x00 + 0x02 + C = 0x03,
    AbsoluteYIndexed().address(0x8000).value(0x02),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x02},
        .final = {
            .a = 0x03,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCAbsoluteYIndexed{
    // 0xFF + 0x01 + C = 0x01,
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ADCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x01},
        .final = {
            .a = 0x01,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true }, // Carry should be set because we wrapped around
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0x01
        }}
}
};

TEST_P(ADCAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteYIndexedAtVariousAddresses,
                         ADCAbsoluteYIndexedMode,
                         testing::ValuesIn(ADCAbsoluteXIndexedModeTestValues) );
