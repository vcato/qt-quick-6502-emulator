#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ADC_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
    NZCVFlags flags;

    uint8_t   addend;
};

using ADCIndirectYIndexed     = ADC<IndirectYIndexed, ADC_IndirectYIndexed_Expectations, 5>;
using ADCIndirectYIndexedMode = ParameterizedInstructionExecutorTestFixture<ADCIndirectYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ADCIndirectYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::IndirectYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;
    fixture.fakeMemory[instruction_param.address.zero_page_address    ]   = fixture.loByteOf(instruction_param.requirements.initial.address_to_indirect_to);
    fixture.fakeMemory[instruction_param.address.zero_page_address + 1]   = fixture.hiByteOf(instruction_param.requirements.initial.address_to_indirect_to);
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.addend;

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
                                 const ADC_IndirectYIndexed_Expectations &expectations)
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
                               const Instruction<AbstractInstruction_e::ADC, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::ADC, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ADCIndirectYIndexed            &instruction)
{
    const uint8_t zero_page_address_to_load_from = instruction.address.zero_page_address;
    const uint8_t value_to_add                   = instruction.requirements.initial.addend;
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t y_register                     = instruction.requirements.initial.y;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from ),    Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page + y_register ), Eq(value_to_add));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const ADCIndirectYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.requirements.initial.address_to_indirect_to);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<ADCIndirectYIndexed> ADCIndirectYIndexedModeTestValues {
ADCIndirectYIndexed{
    // Beginning of a page
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12,
            .flags = { },
            .addend = 6},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = { },
            .addend = 6
        }}
},
ADCIndirectYIndexed{
    // End of a page
    IndirectYIndexed().address(0x10FE).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12,
            .flags = { },
            .addend = 6},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = { },
            .addend = 6
        }}
},
ADCIndirectYIndexed{
    // Crossing a page boundary
    IndirectYIndexed().address(0x10FF).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12,
            .flags = { },
            .addend = 6},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 6,
            .y = 12,
            .flags = { },
            .addend = 6
        }}
},
ADCIndirectYIndexed{
    // Adding a negative affects the N flag
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 12,
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
            .addend = 0xFF
        }}
},
ADCIndirectYIndexed{
    // Adding a zero affects the Z flag
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 0,
            .flags = { },
            .addend = 0},
        .final = {
            .address_to_indirect_to = 0xC000,
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
            .addend = 0xFF
        }}
},
ADCIndirectYIndexed{
    // 1 + 1 = 2, C = 0, V=0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .y = 0,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 2,
            .y = 0,
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
            .addend = 1
        }}
},
ADCIndirectYIndexed{
    // FF + 1 = 0, C = 1, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
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
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCIndirectYIndexed{
    // 1 + FF = 0, C = 1, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .y = 0,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .address_to_indirect_to = 0xC000,
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
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .addend = 0xFF
        }}
},
ADCIndirectYIndexed{
    // 7F + 1 = 80, C = 0, V = 1
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7f,
            .y = 0,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
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
                    .expected_value = true } },
            .addend = 1
        }}
},
ADCIndirectYIndexed{
    // 0x80 + 0x01 = 0x81 (-128 + 1 = -127), C = 0, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = { },
            .addend = 1},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x81,
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
            .addend = 1
        }}
},
ADCIndirectYIndexed{
    // 0x80 + 0x7F = 0xFF (-128 + 127 = -1), C = 0, V = 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = { },
            .addend = 0x7F},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
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
            .addend = 0x7F
        }}
},
ADCIndirectYIndexed{
    // 0x80 + 0x80 = 0x00 (-128 + -128 = -256), C = 1, V = 1
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = { },
            .addend = 0x80},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
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
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = true } },
            .addend = 0x7F
        }}
},
ADCIndirectYIndexed{
    // 0x80 + 0xFF = -129,  V = 1
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0,
            .flags = { },
            .addend = 0xFF},
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .y = 0,
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
            .addend = 0x7F
        }}
},
ADCIndirectYIndexed{
    // 0x00 + 0x02 + C = 0x03,
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x00,
            .y = 0,
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
            .address_to_indirect_to = 0xC000,
            .a = 0x03,
            .y = 0,
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
            .addend = 0x02
        }}
},
ADCIndirectYIndexed{
    // 0xFF + 0x01 + C = 0x01,
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ADCIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 0,
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
            .address_to_indirect_to = 0xC000,
            .a = 0x01,
            .y = 0,
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
            .addend = 0x01
        }}
}
};

TEST_P(ADCIndirectYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadIndirectYIndexedAtVariousAddresses,
                         ADCIndirectYIndexedMode,
                         testing::ValuesIn(ADCIndirectYIndexedModeTestValues) );
