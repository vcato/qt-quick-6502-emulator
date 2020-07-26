#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ASL_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZCFlags flags;

    uint8_t   operand;
};

using ASLZeroPageXIndexed     = ASL<ZeroPageXIndexed, ASL_ZeroPageXIndexed_Expectations, 6>;
using ASLZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<ASLZeroPageXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ASLZeroPageXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPageXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ASL_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ASL, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ASL, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ASLZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.initial.operand));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const ASLZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.final.operand));
}


static const std::vector<ASLZeroPageXIndexed> ADCZeroPageXIndexedModeTestValues {
ASLZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPageXIndexed{
    // Middle of a page
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPageXIndexed{
    // End of a page
    ZeroPageXIndexed().address(0x80FE).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x80FF).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPageXIndexed{
    // Check for High bit going into carry
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 1,
            .x = 0x80,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 1,
            .x = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 0b01010100
        }}
},
ASLZeroPageXIndexed{
    // Check for N flag
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 1,
            .flags = { },
            .operand = 0b11101010 },
        .final = {
            .a = 0xFF,
            .x = 1,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 0b11010100
        }}
},
ASLZeroPageXIndexed{
    // Zero is set in lowest bit
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 1,
            .flags = { },
            .operand = 0b00000001 },
        .final = {
            .a = 0x00,
            .x = 1,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0b00000010
        }}
},
ASLZeroPageXIndexed{
    // Indexing the maximum stays on this page
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    ASLZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0xFF,
            .flags = { },
            .operand = 0b00000001 },
        .final = {
            .a = 0xFF,
            .x = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0b00000010
        }}
}
};

TEST_P(ASLZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ArithmeticShiftLeftZeroPageXIndexedAtVariousAddresses,
                         ASLZeroPageXIndexedMode,
                         testing::ValuesIn(ADCZeroPageXIndexedModeTestValues) );
