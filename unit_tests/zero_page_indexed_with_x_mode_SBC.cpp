#include "addressing_mode_helpers.hpp"



struct SBC_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZCVFlags flags;

    uint8_t   operand;
};

using SBCZeroPageXIndexed     = SBC<ZeroPageXIndexed, SBC_ZeroPageXIndexed_Expectations, 4>;
using SBCZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<SBCZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const SBCZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SBCZeroPageXIndexed &instruction_param)
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
                                                            const SBCZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SBC_ZeroPageXIndexed_Expectations &expectations)
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
                               const Instruction<AbstractInstruction_e::SBC, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SBC, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const SBCZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.initial.operand));
}


static const std::vector<SBCZeroPageXIndexed> SBCZeroPageXIndexedModeTestValues {
SBCZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
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
SBCZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x80FE).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 3,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 3,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x80FF).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 22,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .x = 22,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPageXIndexed{
    // Subtracting a zero does not affect the Z flag
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 6,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .a = 5,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .operand = 0x80
        }}
},
SBCZeroPageXIndexed{
    // 2 - 1 = 1, C = 0, V=0
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x02,
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
SBCZeroPageXIndexed{
    // 0 - 1 = FF, C = 1, V = 0
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
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
SBCZeroPageXIndexed{
    // 0 - FF = 1, C = 1, V = 0
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0xFF },
        .final = {
            .a = 0x01,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
SBCZeroPageXIndexed{
    // 80 - 1 = 7F, C = 0, V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
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
SBCZeroPageXIndexed{
    // 0x81 - 0x01 = 0x80 (-127 - 1 = -128), C = 0, V = 0
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x81,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCZeroPageXIndexed{
    // 0xFF - 0x7F = 0x80 (-1 - 127 = -128), C = 0, V = 0
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
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
SBCZeroPageXIndexed{
    // 0x80 - 0x80 = 0x00 (-128 - -128 = -256), C = 1, V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x80,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCZeroPageXIndexed{
    // 0xFF - 0x80 = -129,  V = 1
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
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
SBCZeroPageXIndexed{
    // 0x03 - 0x02 - C = 0x00,
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x03,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x02 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x02
        }}
},
SBCZeroPageXIndexed{
    // 0x01 - 0x01 - C = 0xFF,
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    SBCZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x01,
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
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
}
};

TEST_P(SBCZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SubtractZeroPageXIndexedAtVariousAddresses,
                         SBCZeroPageXIndexedMode,
                         testing::ValuesIn(SBCZeroPageXIndexedModeTestValues) );
