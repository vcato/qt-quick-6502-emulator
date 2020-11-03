#include "addressing_mode_helpers.hpp"



struct ROR_AbsoluteXIndexed_Expectations
{
    uint8_t x;
    NZCFlags flags;

    uint8_t   operand;
};

using RORAbsoluteXIndexed     = ROR<AbsoluteXIndexed, ROR_AbsoluteXIndexed_Expectations, 7>;
using RORAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<RORAbsoluteXIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const RORAbsoluteXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const RORAbsoluteXIndexed &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const RORAbsoluteXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ROR_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ROR, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ROR, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const RORAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const RORAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq(instruction.requirements.final.operand));
}


static const std::vector<RORAbsoluteXIndexed> RORAbsoluteXIndexedModeTestValues {
RORAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0xA000).value(0xC000),
    RORAbsoluteXIndexed::Requirements{
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
RORAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0xA088).value(0xC000),
    RORAbsoluteXIndexed::Requirements{
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
RORAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0xA0FD).value(0xC000),
    RORAbsoluteXIndexed::Requirements{
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
RORAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0xA0FE).value(0xC000),
    RORAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b00000001 },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b10000000
        }}
},

// Check the bit shift through each bit
RORAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0xA0FF).value(0xC000),
    RORAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b10000000 },
        .final = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b01000000
        }}
},
RORAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    RORAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b01000000 },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00100000
        }}
},
RORAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    RORAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00100000 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00010000
        }}
},
RORAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x80FF).value(0xA0FF),
    RORAbsoluteXIndexed::Requirements{
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
RORAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x80FF).value(0xA0FF),
    RORAbsoluteXIndexed::Requirements{
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
RORAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x80FF).value(0xA0FF),
    RORAbsoluteXIndexed::Requirements{
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
RORAbsoluteXIndexed{
    AbsoluteXIndexed().address(0x80FF).value(0xA0FF),
    RORAbsoluteXIndexed::Requirements{
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

TEST_P(RORAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(RotateRightAbsoluteXIndexedAtVariousAddresses,
                         RORAbsoluteXIndexedMode,
                         testing::ValuesIn(RORAbsoluteXIndexedModeTestValues) );
