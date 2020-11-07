#include "addressing_mode_helpers.hpp"

static inline uint8_t IgnoreCertainBitsInStatus(uint8_t input)
{
        return input | FLAGS6502::B | FLAGS6502::U; // Set this always so we can effectively ignore it during comnparisons
}

struct RTS_Implied_Expectations
{
    uint8_t  stack_pointer;
    uint16_t program_counter_value_on_stack;
};

using RTSImplied     = RTS<Implied, RTS_Implied_Expectations, 6>;
using RTSImpliedMode = ParameterizedInstructionExecutorTestFixture<RTSImplied>;


template <>
void SetupRAMForInstructionsThatHaveImpliedValue(InstructionExecutorTestFixture &fixture,
                                                 const RTSImplied               &instruction_param)
{
    StoreOpcode(fixture, instruction_param);

    // Load values onto stack

    // Program Counter
    fixture.fakeMemory[ fixture.addressUsingStackPointer(instruction_param.requirements.initial.stack_pointer + 1) ] = fixture.loByteOf(instruction_param.requirements.initial.program_counter_value_on_stack);
    fixture.fakeMemory[ fixture.addressUsingStackPointer(instruction_param.requirements.initial.stack_pointer + 2) ] = fixture.hiByteOf(instruction_param.requirements.initial.program_counter_value_on_stack);
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const RTSImplied &instruction_param)
{
    fixture.r.stack_pointer = instruction_param.requirements.initial.stack_pointer;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const RTSImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const RTS_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.stack_pointer));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::RTS, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::RTS, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const RTSImplied                     &instruction)
{
    // Verify that the values to pull are on the stack

    // Program Counter
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer + 1) ), Eq(fixture.loByteOf(instruction.requirements.initial.program_counter_value_on_stack)));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer + 2) ), Eq(fixture.hiByteOf(instruction.requirements.initial.program_counter_value_on_stack)));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const RTSImplied                     &instruction)
{
    // Verify that the memory has not changed

    // Program Counter
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.final.stack_pointer    ) ), Eq(fixture.hiByteOf(instruction.requirements.final.program_counter_value_on_stack)));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.final.stack_pointer - 1) ), Eq(fixture.loByteOf(instruction.requirements.final.program_counter_value_on_stack)));
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor &executor,
                                       const RTSImplied          &instruction)
{
    // One past the instruction, for typical instructions.
    return executor.registers().program_counter == instruction.requirements.final.program_counter_value_on_stack + 1;
}

template<>
void CheckTypicalExecutionResults(const InstructionExecutorTestFixture &fixture,
                                  const RTSImplied                     &instruction)
{
    EXPECT_TRUE(ProgramCounterIsSetToCorrectValue(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    InstructionExecutedInExpectedClockTicks(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.final);
    MemoryContainsExpectedResult(fixture, instruction);
}

// NOTE: OLC emulator always sets U flag to true when executing each instruction.
static const std::vector<RTSImplied> RTSImpliedModeTestValues {
RTSImplied{
// Page ABOVE the stack (beginning)
    Implied().address(0x1000),
    RTSImplied::Requirements{
        .initial = {
            .stack_pointer = 0xFD,
            .program_counter_value_on_stack = 0xAA00
        },
        .final = {
            .stack_pointer = 0xFF,
            .program_counter_value_on_stack = 0xAA00
        }}
},

// Page ABOVE the stack (end)
RTSImplied{
    Implied().address(0x10FF),
    RTSImplied::Requirements{
        .initial = {
            .stack_pointer = 0x00,
            .program_counter_value_on_stack = 0xFF00
        },
        .final = {
            .stack_pointer = 0x02,
            .program_counter_value_on_stack = 0xFF00
        }}
},

// Page ABOVE the stack (middle)
RTSImplied{
    Implied().address(0x1080),
    RTSImplied::Requirements{
        .initial = {
            .stack_pointer = 0x7F,
            .program_counter_value_on_stack = 0xC001
        },
        .final = {
            .stack_pointer = 0x81,
            .program_counter_value_on_stack = 0xC001
        }}
},

// Somewhere else in the page
RTSImplied{
    Implied().address(0x1070),
    RTSImplied::Requirements{
        .initial = {
            .stack_pointer = 0xFF, // Wrap-around
            .program_counter_value_on_stack = 0x2081
        },
        .final = {
            .stack_pointer = 0x01,
            .program_counter_value_on_stack = 0x2081
        }}
}
};

TEST_P(RTSImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ReturnFromSubroutineImpliedAtVariousAddresses,
                         RTSImpliedMode,
                         testing::ValuesIn(RTSImpliedModeTestValues) );
