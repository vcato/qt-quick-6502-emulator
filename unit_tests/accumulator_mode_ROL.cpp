#include "addressing_mode_helpers.hpp"



struct ROL_Accumulator_Expectations
{
    constexpr ROL_Accumulator_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t  a;
    NZCFlags flags;
};

using ROLAccumulator     = ROL<Accumulator, ROL_Accumulator_Expectations, 2>;
using ROLAccumulatorMode = ParameterizedInstructionExecutorTestFixture<ROLAccumulator>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ROLAccumulator &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ROLAccumulator                 &instruction_param)
{
    SetupRAMForInstructionsThatHaveNoEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ROL_Accumulator_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ROL, Accumulator> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ROL, AddressMode_e::Accumulator) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const ROLAccumulator                 &)
{
    // No memory is affected
}


static const std::vector<ROLAccumulator> ROLAccumulatorModeTestValues {
ROLAccumulator{
    // Beginning of a page
    // Demonstrate the carry bit behavior
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0,
            .flags = { } },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    // Middle of a page
    // Demonstrate the carry bit behavior
    Accumulator().address(0x8080),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } } },
        .final = {
            .a = 0b00000001,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    // End of a page
    // Demonstrate the carry bit behavior
    Accumulator().address(0x80FF),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b10000000,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
        }}
},
ROLAccumulator{
    // End of a page
    // Demonstrate the carry bit behavior
    Accumulator().address(0x80FF),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b10000000,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } } },
        .final = {
            .a = 0b00000001,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
},

// Check the bit shift through each bit
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b00000001,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b00000010,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b00000010,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b00000100,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b00000100,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b00001000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b00001000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b00010000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b00010000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b00100000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b00100000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b01000000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
ROLAccumulator{
    Accumulator().address(0x8000),
    ROLAccumulator::Requirements{
        .initial = {
            .a = 0b01000000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } } },
        .final = {
            .a = 0b10000000,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
}
};

TEST_P(ROLAccumulatorMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(RotateLeftAccumulatorAtVariousAddresses,
                         ROLAccumulatorMode,
                         testing::ValuesIn(ROLAccumulatorModeTestValues) );
