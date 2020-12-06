#include "addressing_mode_helpers.hpp"

static inline uint8_t IgnoreCertainBitsInStatus(uint8_t input)
{
        return input | FLAGS6502::B | FLAGS6502::U; // Set this always so we can effectively ignore it during comnparisons
}

struct RTI_Implied_Expectations
{
    uint8_t status_register;
    uint8_t stack_pointer;
    uint8_t status_register_value_on_stack;
    uint16_t program_counter_value_on_stack;
};

using RTIImplied     = RTI<Implied, RTI_Implied_Expectations, 6>;
using RTIImpliedMode = ParameterizedInstructionExecutorTestFixture<RTIImplied>;


template <>
void SetupRAMForInstructionsThatHaveImpliedValue(InstructionExecutorTestFixture &fixture,
                                                 const RTIImplied               &instruction_param)
{
    StoreOpcode(fixture, instruction_param);

    // Load values onto stack

    // Status Register
    fixture.fakeMemory[ fixture.addressUsingStackPointer(instruction_param.requirements.initial.stack_pointer + 1) ] = instruction_param.requirements.initial.status_register_value_on_stack;

    // Program Counter
    fixture.fakeMemory[ fixture.addressUsingStackPointer(instruction_param.requirements.initial.stack_pointer + 2) ] = fixture.loByteOf(instruction_param.requirements.initial.program_counter_value_on_stack);
    fixture.fakeMemory[ fixture.addressUsingStackPointer(instruction_param.requirements.initial.stack_pointer + 3) ] = fixture.hiByteOf(instruction_param.requirements.initial.program_counter_value_on_stack);
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const RTIImplied &instruction_param)
{
    fixture.r.status = instruction_param.requirements.initial.status_register;
    fixture.r.stack_pointer = instruction_param.requirements.initial.stack_pointer;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const RTIImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const RTI_Implied_Expectations &expectations)
{
    uint8_t register_status = IgnoreCertainBitsInStatus(registers.status);
    uint8_t expectation_status = IgnoreCertainBitsInStatus(expectations.status_register);

    EXPECT_THAT(register_status, Eq(expectation_status));
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.stack_pointer));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::RTI, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::RTI, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const RTIImplied                     &instruction)
{
    // Verify that the values to pull are on the stack

    // Status Register
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer + 1) ), Eq( instruction.requirements.initial.status_register_value_on_stack ));

    // Program Counter
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer + 2) ), Eq(fixture.loByteOf(instruction.requirements.initial.program_counter_value_on_stack)));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer + 3) ), Eq(fixture.hiByteOf(instruction.requirements.initial.program_counter_value_on_stack)));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const RTIImplied                     &instruction)
{
    // Verify that the memory has not changed

    // Program Counter
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.final.stack_pointer    ) ), Eq(fixture.hiByteOf(instruction.requirements.final.program_counter_value_on_stack)));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.final.stack_pointer - 1) ), Eq(fixture.loByteOf(instruction.requirements.final.program_counter_value_on_stack)));

    // Status Register
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.final.stack_pointer - 2) ), Eq( instruction.requirements.final.status_register_value_on_stack ));
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor &executor,
                                       const RTIImplied          &instruction)
{
    // One past the instruction, for typical instructions.
    return executor.registers().program_counter == instruction.requirements.final.program_counter_value_on_stack;
}

template<>
void CheckTypicalExecutionResults(const InstructionExecutorTestFixture &fixture,
                                  const RTIImplied                     &instruction)
{
    EXPECT_TRUE(ProgramCounterIsSetToCorrectValue(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    InstructionExecutedInExpectedClockTicks(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.final);
    MemoryContainsExpectedResult(fixture, instruction);
}

// NOTE: OLC emulator always sets U flag to true when executing each instruction.
static const std::vector<RTIImplied> RTIImpliedModeTestValues {
RTIImplied{
// Page ABOVE the stack (beginning)
    Implied().address(0x1000),
    RTIImplied::Requirements{
        .initial = {
            .status_register = 0,
            .stack_pointer = 0xFC,
            .status_register_value_on_stack = 0xFF,
            .program_counter_value_on_stack = 0xAA00
        },
        .final = {
            .status_register = 0xFF,
            .stack_pointer = 0xFF,
            .status_register_value_on_stack = 0xFF,
            .program_counter_value_on_stack = 0xAA00
        }}
},

// Page ABOVE the stack (end)
RTIImplied{
    Implied().address(0x10FF),
    RTIImplied::Requirements{
        .initial = {
            .status_register = 0xFF,
            .stack_pointer = 0x00,
            .status_register_value_on_stack = 0x00,
            .program_counter_value_on_stack = 0xFF00
        },
        .final = {
            .status_register = 0x00,
            .stack_pointer = 0x03,
            .status_register_value_on_stack = 0x00,
            .program_counter_value_on_stack = 0xFF00
        }}
},

// Page ABOVE the stack (middle)
RTIImplied{
    Implied().address(0x1080),
    RTIImplied::Requirements{
        .initial = {
            .status_register = 0b10101010,
            .stack_pointer = 0x7F,
            .status_register_value_on_stack = 0x80,
            .program_counter_value_on_stack = 0xC001
        },
        .final = {
            .status_register = 0x80,
            .stack_pointer = 0x82,
            .status_register_value_on_stack = 0x80,
            .program_counter_value_on_stack = 0xC001
        }}
},

// Somewhere else in the page
RTIImplied{
    Implied().address(0x1070),
    RTIImplied::Requirements{
        .initial = {
            .status_register = 0b01010101,
            .stack_pointer = 0xFF, // Wrap-around
            .status_register_value_on_stack = 0b10101010,
            .program_counter_value_on_stack = 0x2081
        },
        .final = {
            .status_register = 0b10101010,
            .stack_pointer = 0x02,
            .status_register_value_on_stack = 0b10101010,
            .program_counter_value_on_stack = 0x2081
        }}
}
};

TEST_P(RTIImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ReturnFromInterruptImpliedAtVariousAddresses,
                         RTIImpliedMode,
                         testing::ValuesIn(RTIImpliedModeTestValues) );
