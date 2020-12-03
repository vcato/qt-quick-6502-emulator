#include "addressing_mode_helpers.hpp"



struct SBC_AbsoluteXIndexed_Expectations
{
    constexpr SBC_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t x;
    NZCVFlags flags;

    uint8_t   operand;
};

using SBCAbsoluteXIndexed     = SBC<AbsoluteXIndexed, SBC_AbsoluteXIndexed_Expectations, 4>;
using SBCAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<SBCAbsoluteXIndexed>;



static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const SBCAbsoluteXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SBCAbsoluteXIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(InstructionExecutorTestFixture &fixture,
                                                            const SBCAbsoluteXIndexed      &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SBC_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::SBC, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SBC, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const SBCAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const SBCAbsoluteXIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.x;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}

static const std::vector<SBCAbsoluteXIndexed> SBCAbsoluteXIndexedModeTestValues {
SBCAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0088).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    // Subtracting a zero does not affect the Z flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous overflow occurred.  Simulate borrow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 5,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow generated
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false }, // Previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = true } },
            .operand = 0x80
        }}
},
// Carry flag
SBCAbsoluteXIndexed{
    // 2 - 1 = 1, C = 0, V=0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x02,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x01,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCAbsoluteXIndexed{
    // 0 - 1 = FF, C = 1, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCAbsoluteXIndexed{
    // 0 - FF = 1, C = 1, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0xFF },
        .final = {
            .a = 0x01,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
SBCAbsoluteXIndexed{
    // 80 - 1 = 7F, C = 0, V = 1
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
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
},
SBCAbsoluteXIndexed{
    // 0x81 - 0x01 = 0x80 (-127 - 1 = -128), C = 0, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x81,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCAbsoluteXIndexed{
    // 0xFF - 0x7F = 0x80 (-1 - 127 = -128), C = 0, V = 0
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x7F },
        .final = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x7F
        }}
},
SBCAbsoluteXIndexed{
    // 0x80 - 0x80 = 0x00 (-128 - -128 = -256), C = 1, V = 1
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCAbsoluteXIndexed{
    // 0xFF - 0x80 = -129,  V = 1
    AbsoluteXIndexed().address(0x8000).value(0x02),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x7F,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCAbsoluteXIndexed{
    // 0x03 - 0x02 - C = 0x00,
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x03,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x02 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x02
        }}
},
SBCAbsoluteXIndexed{
    // 0x01 - 0x01 - C = 0xFF,
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    SBCAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
}
};

TEST_P(SBCAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SubtractAbsoluteXIndexedAtVariousAddresses,
                         SBCAbsoluteXIndexedMode,
                         testing::ValuesIn(SBCAbsoluteXIndexedModeTestValues) );
