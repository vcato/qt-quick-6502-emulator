#include "addressing_mode_helpers.hpp"



struct ROL_Absolute_Expectations
{
    NZCFlags flags;
    uint8_t  operand;
};

using ROLAbsolute     = ROL<Absolute, ROL_Absolute_Expectations, 6>;
using ROLAbsoluteMode = ParameterizedInstructionExecutorTestFixture<ROLAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ROLAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ROLAbsolute &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ROLAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ROL_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ROL, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ROL, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ROLAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const ROLAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq(instruction.requirements.final.operand));
}

static const std::vector<ROLAbsolute> ROLAbsoluteModeTestValues {
ROLAbsolute{
    // Beginning of a page
    Absolute().address(0x4000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ROLAbsolute{
    // Middle of a page
    Absolute().address(0x4088).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000001
        }}
},
ROLAbsolute{
    // End of a page
    Absolute().address(0x40FD).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b10000000 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
ROLAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x40FE).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b10000000 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b00000001
        }}
},
ROLAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x40FF).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000001 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010
        }}
},
ROLAbsolute{
    Absolute().address(0x8000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000100
        }}
},
ROLAbsolute{
    Absolute().address(0x8000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000100 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00001000
        }}
},
ROLAbsolute{
    Absolute().address(0x8000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00001000 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00010000
        }}
},
ROLAbsolute{
    Absolute().address(0x8000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00010000 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00100000
        }}
},
ROLAbsolute{
    Absolute().address(0x8000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00100000 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b01000000
        }}
},
ROLAbsolute{
    Absolute().address(0x8000).value(0xA000),
    ROLAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b01000000 },
        .final = {
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b10000000
        }}
}
};

TEST_P(ROLAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(RotateLeftAbsoluteAtVariousAddresses,
                         ROLAbsoluteMode,
                         testing::ValuesIn(ROLAbsoluteModeTestValues) );
