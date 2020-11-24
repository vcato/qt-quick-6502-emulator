#include "addressing_mode_helpers.hpp"



struct SBC_AbsoluteYIndexed_Expectations
{
    uint8_t a;
    uint8_t y;
    NZCVFlags flags;

    uint8_t   operand;
};

using SBCAbsoluteYIndexed     = SBC<AbsoluteYIndexed, SBC_AbsoluteYIndexed_Expectations, 4>;
using SBCAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<SBCAbsoluteYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const SBCAbsoluteYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.y ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SBCAbsoluteYIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const SBCAbsoluteYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SBC_AbsoluteYIndexed_Expectations &expectations)
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
                               const Instruction<AbstractInstruction_e::SBC, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SBC, AddressMode_e::AbsoluteYIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const SBCAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.y ), Eq( instruction.requirements.initial.operand ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const SBCAbsoluteYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<SBCAbsoluteYIndexed> SBCAbsoluteYIndexedModeTestValues {
SBCAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0
        }}
},
SBCAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 5,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 6,
            .y = 5,
            .flags = { },
            .operand = 0
        }}
},
SBCAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0
        }}
},
SBCAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0
        }}
},
SBCAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 6,
            .y = 0,
            .flags = { },
            .operand = 0
        }}
},
SBCAbsoluteYIndexed{
    // Subtracting a zero does not affect the Z flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 6,
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .a = 6,
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCAbsoluteYIndexed{
    // Subtracting a zero does not affect the Z flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .a = 0x00,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCAbsoluteYIndexed{
    // Subtracting a negative affects the N flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
// Carry flag
SBCAbsoluteYIndexed{
    // 2 - 1 = 1, C = 0, V=0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x02,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x01,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCAbsoluteYIndexed{
    // 0 - 1 = FF, C = 1, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0xFF,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCAbsoluteYIndexed{
    // 0 - FF = 1, C = 1, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0xFF },
        .final = {
            .a = 0x01,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
SBCAbsoluteYIndexed{
    // 80 - 1 = 7F, C = 0, V = 1
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x7F,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
},
SBCAbsoluteYIndexed{
    // 0x81 - 0x01 = 0x80 (-127 - 1 = -128), C = 0, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x81,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x80,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCAbsoluteYIndexed{
    // 0xFF - 0x7F = 0x80 (-1 - 127 = -128), C = 0, V = 0
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x7F },
        .final = {
            .a = 0x80,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x7F
        }}
},
SBCAbsoluteYIndexed{
    // 0x80 - 0x80 = 0x00 (-128 - -128 = -256), C = 1, V = 1
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x00,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .operand = 0x80
        }}
},
SBCAbsoluteYIndexed{
    // 0xFF - 0x80 = -129,  V = 1
    AbsoluteYIndexed().address(0x8000).value(0x02),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x7F,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .operand = 0x80
        }}
},
SBCAbsoluteYIndexed{
    // 0x03 - 0x02 - C = 0x00,
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x03,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x02 },
        .final = {
            .a = 0x00,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false }, // Carry should be set because we wrapped around
                .v_value = { .expected_value = false } },
            .operand = 0x02
        }}
},
SBCAbsoluteYIndexed{
    // 0x01 - 0x01 - C = 0xFF,
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0xFF,
            .y = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
}
};

TEST_P(SBCAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SubtractAbsoluteYIndexedAtVariousAddresses,
                         SBCAbsoluteYIndexedMode,
                         testing::ValuesIn(SBCAbsoluteYIndexedModeTestValues) );
