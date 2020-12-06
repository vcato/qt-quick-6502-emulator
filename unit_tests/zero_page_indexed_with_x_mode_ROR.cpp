#include "addressing_mode_helpers.hpp"



struct ROR_ZeroPageXIndexed_Expectations
{
    uint8_t x;
    NZCFlags flags;

    uint8_t  operand;
};

using RORZeroPageXIndexed     = ROR<ZeroPageXIndexed, ROR_ZeroPageXIndexed_Expectations, 6>;
using RORZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<RORZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const RORZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const RORZeroPageXIndexed &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const RORZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ROR_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ROR, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ROR, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const RORZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.initial.operand));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const RORZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.final.operand));
}


static const std::vector<RORZeroPageXIndexed> RORZeroPageXIndexedModeTestValues {
RORZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x8000).zp_address(6),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
RORZeroPageXIndexed{
    // Middle of a page
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b10000000
        }}
},
RORZeroPageXIndexed{
    // End of a page
    ZeroPageXIndexed().address(0x80FE).zp_address(6),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000001 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
RORZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x80FF).zp_address(6),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b00000001 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b10000000
        }}
},

// Check the bit shift through each bit
RORZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b10000000 },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b01000000
        }}
},
RORZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(6),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b01000000 },
        .final = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00100000
        }}
},
RORZeroPageXIndexed{
    // Indexing the maximum stays on this page
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00100000 },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00010000
        }}
},
RORZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00010000 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00001000
        }}
},
RORZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00001000 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000100
        }}
},
RORZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000100 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010
        }}
},
RORZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8080).zp_address(0x80),
    RORZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000001
        }}
}
};

TEST_P(RORZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(RotateRightZeroPageXIndexedAtVariousAddresses,
                         RORZeroPageXIndexedMode,
                         testing::ValuesIn(RORZeroPageXIndexedModeTestValues) );
