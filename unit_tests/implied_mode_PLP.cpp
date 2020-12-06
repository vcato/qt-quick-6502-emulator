#include "addressing_mode_helpers.hpp"

static inline uint8_t IgnoreCertainBitsInStatus(uint8_t input)
{
        return input | FLAGS6502::B | FLAGS6502::U; // Set this always so we can effectively ignore it during comnparisons
}

struct PLP_Implied_Expectations
{
    uint8_t status_register;
    uint8_t stack_pointer;
    uint8_t value_on_stack;
};

using PLPImplied     = PLP<Implied, PLP_Implied_Expectations, 4>;
using PLPImpliedMode = ParameterizedInstructionExecutorTestFixture<PLPImplied>;


template <>
void SetupRAMForInstructionsThatHaveImpliedValue(InstructionExecutorTestFixture &fixture,
                                                 const PLPImplied               &instruction_param)
{
    StoreOpcode(fixture, instruction_param);

    // Load value onto stack
    fixture.fakeMemory[ fixture.addressUsingStackPointer(instruction_param.requirements.initial.stack_pointer + 1) ] = instruction_param.requirements.initial.value_on_stack;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const PLPImplied &instruction_param)
{
    fixture.r.status = instruction_param.requirements.initial.status_register;
    fixture.r.stack_pointer = instruction_param.requirements.initial.stack_pointer;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const PLPImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const PLP_Implied_Expectations &expectations)
{
    uint8_t register_status = IgnoreCertainBitsInStatus(registers.status);
    uint8_t expectation_status = IgnoreCertainBitsInStatus(expectations.status_register);

    EXPECT_THAT(register_status, Eq(expectation_status));
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.stack_pointer));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::PLP, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::PLP, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const PLPImplied                     &instruction)
{
    // Verify that the value to pull is on the stack
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer + 1) ), Eq( instruction.requirements.initial.value_on_stack ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const PLPImplied                     &instruction)
{
    // Verify that the memory has not changed
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.final.stack_pointer) ), Eq( instruction.requirements.initial.value_on_stack ));
}

// NOTE: OLC emulator always sets U flag to true when executing each instruction.
static const std::vector<PLPImplied> PLPImpliedModeTestValues {
PLPImplied{
// Page ABOVE the stack (beginning)
    Implied().address(0x1000),
    PLPImplied::Requirements{
        .initial = {
            .status_register = 0,
            .stack_pointer = 0xFE,
            .value_on_stack = 0xFF
        },
        .final = {
            .status_register = 0xFF,
            .stack_pointer = 0xFF,
            .value_on_stack = 0xFF
        }}
},

// Page ABOVE the stack (end)
PLPImplied{
    Implied().address(0x10FF),
    PLPImplied::Requirements{
        .initial = {
            .status_register = 0xFF,
            .stack_pointer = 0x00,
            .value_on_stack = 0x00
        },
        .final = {
            .status_register = 0x00,
            .stack_pointer = 0x01,
            .value_on_stack = 0x00
        }}
},

// Page ABOVE the stack (middle)
PLPImplied{
    Implied().address(0x1080),
    PLPImplied::Requirements{
        .initial = {
            .status_register = 0b10101010,
            .stack_pointer = 0xFF,
            .value_on_stack = 0x80
        },
        .final = {
            .status_register = 0x80,
            .stack_pointer = 0x00,
            .value_on_stack = 0x80
        }}
},

// Somewhere else in the page
PLPImplied{
    Implied().address(0x1070),
    PLPImplied::Requirements{
        .initial = {
            .status_register = 0b01010101,
            .stack_pointer = 0x7F,
            .value_on_stack = 0x7F
        },
        .final = {
            .status_register = 0x7F,
            .stack_pointer = 0x80,
            .value_on_stack = 0x7F
        }}
}
};

TEST_P(PLPImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(PullProcessorStatusFromStackImpliedAtVariousAddresses,
                         PLPImpliedMode,
                         testing::ValuesIn(PLPImpliedModeTestValues) );
