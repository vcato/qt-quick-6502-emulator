#include "addressing_mode_helpers.hpp"



struct SBC_ZeroPage_Expectations
{
    uint8_t   a;
    NZCVFlags flags;

    uint8_t   operand;
};

using SBCZeroPage     = SBC<ZeroPage, SBC_ZeroPage_Expectations, 3>;
using SBCZeroPageMode = ParameterizedInstructionExecutorTestFixture<SBCZeroPage>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const SBCZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SBCZeroPage &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const SBCZeroPage                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SBC_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::SBC, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SBC, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const SBCZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.operand));
}


static const std::vector<SBCZeroPage> SBCZeroPageModeTestValues {
SBCZeroPage{
    // Beginning of a page
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPage{
    // One before the end of a page
    ZeroPage().address(0x80FE).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x80FF).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPage{
    // Subtracting a zero does not affect the Z flag
    ZeroPage().address(0x8080).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 6,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous overflow occurred.  Simulate borrow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow generated
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPage{
    ZeroPage().address(0x8080).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCZeroPage{
    ZeroPage().address(0x8080).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false }, // Previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCZeroPage{
    ZeroPage().address(0x8080).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No Previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = true } },
            .operand = 0x80
        }}
},
// Carry flag
SBCZeroPage{
    // 2 - 1 = 1, C = 0, V=0
    ZeroPage().address(0x8080).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x02,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCZeroPage{
    // 0 - 1 = FF, C = 1, V = 0
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCZeroPage{
    // 0 - FF = 1, C = 1, V = 0
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0xFF },
        .final = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
SBCZeroPage{
    // 80 - 1 = 7F, C = 0, V = 1
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
},
SBCZeroPage{
    // 0x81 - 0x01 = 0x80 (-127 - 1 = -128), C = 0, V = 0
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x81,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCZeroPage{
    // 0xFF - 0x7F = 0x80 (-1 - 127 = -128), C = 0, V = 0
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x7F },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x7F
        }}
},
SBCZeroPage{
    // 0x80 - 0x80 = 0x00 (-128 - -128 = -256), C = 1, V = 1
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCZeroPage{
    // 0xFF - 0x80 = -129,  V = 1
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No previous borrow
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .a = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCZeroPage{
    // 0x03 - 0x02 - C = 0x00,
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x03,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous borrow occurred
                .v_value = { .expected_value = false } },
                .operand = 0x02 },
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true }, // No borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x02
        }}
},
SBCZeroPage{
    // 0x01 - 0x01 - C = 0xFF,
    ZeroPage().address(0x8000).zp_address(6),
    SBCZeroPage::Requirements{
        .initial = {
            .a = 0x01,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous borrow occurred
                .v_value = { .expected_value = false } },
                .operand = 0x01 },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Borrow occurred
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
}
};

TEST_P(SBCZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SubtractZeroPageAtVariousAddresses,
                         SBCZeroPageMode,
                         testing::ValuesIn(SBCZeroPageModeTestValues) );
