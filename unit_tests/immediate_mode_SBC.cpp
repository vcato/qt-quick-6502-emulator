#include "addressing_mode_helpers.hpp"



struct SBC_Immediate_Expectations
{
    uint8_t   a;
    NZCVFlags flags;
};

using SBCImmediate     = SBC<Immediate, SBC_Immediate_Expectations, 2>;
using SBCImmediateMode = ParameterizedInstructionExecutorTestFixture<SBCImmediate>;


static void StoreTestValueAtImmediateAddress(InstructionExecutorTestFixture &fixture, const SBCImmediate &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SBCImmediate &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const SBCImmediate                   &instruction_param)
{
    SetupRAMForInstructionsThatHaveImmediateValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SBC_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::SBC, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SBC, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const SBCImmediate                   &/* instruction */)
{
}


static const std::vector<SBCImmediate> SBCImmediateModeTestValues {
SBCImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(0),
    SBCImmediate::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
SBCImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(0),
    SBCImmediate::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
SBCImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(0),
    SBCImmediate::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
SBCImmediate{
    // Subtracting a zero does not affect the Z flag
    Immediate().address(0x8000).value(0x00),
    SBCImmediate::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // Subtracting a zero does not affect the Z flag
    Immediate().address(0x8000).value(0x00),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // Subtracting a negative affects the N flag
    Immediate().address(0x8000).value(0x80),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
// Carry flag
SBCImmediate{
    // 2 - 1 = 1, C = 0, V=0
    Immediate().address(0x8000).value(0x01),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x02,
            .flags = { }},
        .final = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // 0 - 1 = FF, C = 1, V = 0
    Immediate().address(0x8000).value(0x01),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // 0 - FF = 1, C = 1, V = 0
    Immediate().address(0x8000).value(0xFF),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { }},
        .final = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // 80 - 1 = 7F, C = 0, V = 1
    Immediate().address(0x8000).value(0x01),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0x7f,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } }
        }}
},
SBCImmediate{
    // 0x81 - 0x01 = 0x80 (-127 - 1 = -128), C = 0, V = 0
    Immediate().address(0x8000).value(0x01),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x81,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // 0xFF - 0x7F = 0x80 (-1 - 127 = -128), C = 0, V = 0
    Immediate().address(0x8000).value(0x7F),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // 0x80 - 0x80 = 0x00 (-128 - -128 = -256), C = 1, V = 1
    Immediate().address(0x8000).value(0x80),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x80,
            .flags = { }},
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } }
        }}
},
SBCImmediate{
    // 0xFF - 0x80 = -129,  V = 1
    Immediate().address(0x8000).value(0x80),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } }
        }}
},
SBCImmediate{
    // 0x03 - 0x02 - C = 0x00,
    Immediate().address(0x8000).value(0x02),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x03,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } }
        }}
},
SBCImmediate{
    // 0x01 - 0x01 - C = 0xFF,
    Immediate().address(0x8000).value(0x01),
    SBCImmediate::Requirements{
        .initial = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } } },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry should be set because we wrapped around
                .v_value = { .expected_value = false } }
        }}
}
};


TEST_P(SBCImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SubtractImmediateAtVariousAddresses,
                         SBCImmediateMode,
                         testing::ValuesIn(SBCImmediateModeTestValues) );
