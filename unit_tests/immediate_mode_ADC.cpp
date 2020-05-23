#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ADC_Immediate_Expectations
{
    constexpr ADC_Immediate_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t   a;
    NZCVFlags flags;
};

using ADCImmediate     = ADC<Immediate, ADC_Immediate_Expectations, 2>;
using ADCImmediateMode = ParameterizedInstructionExecutorTestFixture<ADCImmediate>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ADCImmediate                   &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Immediate,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ADC_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ADC, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const ADCImmediate                   &/* instruction */)
{
}


static const std::vector<ADCImmediate> ADCImmediateModeTestValues {
// Test memory transfer
ADCImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(6),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
ADCImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(6),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
ADCImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(6),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
ADCImmediate{
    // Adding a zero does not affect the Z flag
    Immediate().address(0x8000).value(0x00),
    ADCImmediate::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 6,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // Adding a negative affects the N flag
    Immediate().address(0x8000).value(0x80),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // Rolling over affects the Z and C flags
    Immediate().address(0x8000).value(0x01),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { }},
        .final = {
            .a = 0x00,
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
                    .expected_value = false } }
        }}
},

// Carry flag
ADCImmediate{
    // 1 + 1 = 2, C = 0, V=0
    Immediate().address(0x8000).value(0x01),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x01,
            .flags = { }},
        .final = {
            .a = 0x02,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // FF + 1 = 0, C = 1, V = 0
    Immediate().address(0x8000).value(0x01),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { }},
        .final = {
            .a = 0x00,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // 1 + FF = 0, C = 1, V = 0
    Immediate().address(0x8000).value(0xFF),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x01,
            .flags = { }},
        .final = {
            .a = 0x00,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // 7F + 1 = 80, C = 0, V = 1
    Immediate().address(0x8000).value(0x01),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x7f,
            .flags = { }},
        .final = {
            .a = 0x80,
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
                    .expected_value = true } }
        }}
},
ADCImmediate{
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    Immediate().address(0x8000).value(0x01),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { }},
        .final = {
            .a = 0x81,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    Immediate().address(0x8000).value(0x7F),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { }},
        .final = {
            .a = 0xFF,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // 0x80 + 0x80 = 0x00 (-128 + -127 = -256), C = 1, V = 1
    Immediate().address(0x8000).value(0x80),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { }},
        .final = {
            .a = 0x00,
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
                    .expected_value = true } }
        }}
},
ADCImmediate{
    // 0x80 + 0xFF = -129,  V = 1
    Immediate().address(0x8000).value(0xFF),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x80,
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
                    .expected_value = false } } },
        .final = {
            .a = 0x7F,
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
                    .expected_value = true } }
        }}
},
ADCImmediate{
    // 0x00 + 0x02 + C = 0x03,
    Immediate().address(0x8000).value(0x02),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0x00,
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
                    .expected_value = false } } },
        .final = {
            .a = 0x03,
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
                    .expected_value = false } }
        }}
},
ADCImmediate{
    // 0xFF + 0x01 + C = 0x01,
    Immediate().address(0x8000).value(0x01),
    ADCImmediate::Requirements{
        .initial = {
            .a = 0xFF,
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
                    .expected_value = false } } },
        .final = {
            .a = 0x01,
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
                    .expected_value = false } }
        }}
}
};


TEST_P(ADCImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddImmediateAtVariousAddresses,
                         ADCImmediateMode,
                         testing::ValuesIn(ADCImmediateModeTestValues) );

