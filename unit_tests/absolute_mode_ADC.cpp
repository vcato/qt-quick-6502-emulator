#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ADC_Absolute_Expectations
{
    constexpr ADC_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZCVFlags flags;

    uint8_t   addend;
};

using ADCAbsolute     = ADC<Absolute, ADC_Absolute_Expectations, 4>;
using ADCAbsoluteMode = ParameterizedInstructionExecutorTestFixture<ADCAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ADCAbsolute                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Absolute,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.addend;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ADC, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.addend ));
}


static const std::vector<ADCAbsolute> ADCAbsoluteModeTestValues {
ADCAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .addend = 6},
        .final = {
            .a = 6,
            .flags = { },
            .addend = 6
        }}
},
ADCAbsolute{
    // Adding a zero affects the Z flag
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { },
            .addend = 0},
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0
        }}
},
ADCAbsolute{
    // Adding a negative affects the N flag
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { },
            .addend = 0x80},
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
// Carry flag
ADCAbsolute{
    // 1 + 1 = 2, C = 0, V=0
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x01,
            .flags = { },
            .addend = 0x01},
        .final = {
            .a = 0x02,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
},
ADCAbsolute{
    // FF + 1 = 0, C = 1, V = 0
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .addend = 0x01},
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
ADCAbsolute{
    // 1 + FF = 0, C = 1, V = 0
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x01,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCAbsolute{
    // 7F + 1 = 80, C = 0, V = 1
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x7f,
            .flags = { },
            .addend = 0x01},
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
ADCAbsolute{
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { },
            .addend = 0x01},
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
ADCAbsolute{
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { },
            .addend = 0x7F},
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0x7F
        }}
},
ADCAbsolute{
    // 0x80 + 0x80 = 0x00 (-128 + -127 = -256), C = 1, V = 1
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { },
            .addend = 0x80},
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0x80
        }}
},
ADCAbsolute{
    // 0x80 + 0xFF = -129,  V = 1
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .addend = 0xFF
        }}
},
ADCAbsolute{
    // 0x00 + 0x02 + C = 0x03,
    Absolute().address(0x8000).value(0x02),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x02},
        .final = {
            .a = 0x03,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCAbsolute{
    // 0xFF + 0x01 + C = 0x01,
    Absolute().address(0x8000).value(0xA000),
    ADCAbsolute::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .addend = 0x01},
        .final = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry should be set because we wrapped around
                .v_value = { .expected_value = false } },
            .addend = 0x01
        }}
}
};

TEST_P(ADCAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddAbsoluteAtVariousAddresses,
                         ADCAbsoluteMode,
                         testing::ValuesIn(ADCAbsoluteModeTestValues) );
