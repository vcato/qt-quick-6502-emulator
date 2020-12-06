#include "addressing_mode_helpers.hpp"



struct SBC_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
    NZCVFlags flags;

    uint8_t   operand;
};

using SBCIndirectYIndexed     = SBC<IndirectYIndexed, SBC_IndirectYIndexed_Expectations, 5>;
using SBCIndirectYIndexedMode = ParameterizedInstructionExecutorTestFixture<SBCIndirectYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const SBCIndirectYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SBCIndirectYIndexed &instruction_param)
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
                                                            const SBCIndirectYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SBC_IndirectYIndexed_Expectations &expectations)
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
                               const Instruction<AbstractInstruction_e::SBC, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::SBC, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const SBCIndirectYIndexed            &instruction)
{
    const uint8_t zero_page_address_to_load_from = instruction.address.zero_page_address;
    const uint8_t value_to_add                   = instruction.requirements.initial.operand;
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t y_register                     = instruction.requirements.initial.y;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from ),    Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page + y_register ), Eq(value_to_add));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const SBCIndirectYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.requirements.initial.address_to_indirect_to);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = (fixture.hiByteOf(effective_address) - fixture.hiByteOf(original_address)) > 0x00;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<SBCIndirectYIndexed> SBCIndirectYIndexedModeTestValues {
SBCIndirectYIndexed{
    // Beginning of a page
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCIndirectYIndexed{
    // End of a page
    IndirectYIndexed().address(0x10FE).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // Carry bit is clear on overflow. So, simulate no overflow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCIndirectYIndexed{
    // Crossing a page boundary
    IndirectYIndexed().address(0x10FF).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false }, // Previous overflow occurred.  Simulate borrow.
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 5,
            .y = 12,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true }, // No borrow!
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCIndirectYIndexed{
    // Subtracting a zero does not affect the Z flag
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0
        }}
},
SBCIndirectYIndexed{
    // Subtracting a zero does not affect the Z flag
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x00
        }}
},
SBCIndirectYIndexed{
    // Subtracting a negative affects the N flag
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = true } },
            .operand = 0x80
        }}
},
SBCIndirectYIndexed{
    // 2 - 1 = 1, C = 0, V=0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x02,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x01,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCIndirectYIndexed{
    // 0 - 1 = FF, C = 1, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCIndirectYIndexed{
    // 0 - FF = 1, C = 1, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0xFF },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x01,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
SBCIndirectYIndexed{
    // 80 - 1 = 7F, C = 0, V = 1
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .operand = 0x01
        }}
},
SBCIndirectYIndexed{
    // 0x81 - 0x01 = 0x80 (-127 - 1 = -128), C = 0, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x81,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
},
SBCIndirectYIndexed{
    // 0xFF - 0x7F = 0x80 (-1 - 127 = -128), C = 0, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x7F },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x7F
        }}
},
SBCIndirectYIndexed{
    // 0x80 - 0x80 = 0x00 (-128 - -128 = -256), C = 1, V = 1
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCIndirectYIndexed{
    // 0xFF - 0x80 = -129,  V = 1
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
SBCIndirectYIndexed{
    // 0x03 - 0x02 - C = 0x00,
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x03,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x02 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0x02
        }}
},
SBCIndirectYIndexed{
    // 0x01 - 0x01 - C = 0xFF,
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    SBCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x01,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0x01
        }}
}
};

TEST_P(SBCIndirectYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SubtractIndirectYIndexedAtVariousAddresses,
                         SBCIndirectYIndexedMode,
                         testing::ValuesIn(SBCIndirectYIndexedModeTestValues) );
