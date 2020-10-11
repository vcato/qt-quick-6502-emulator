#include "addressing_mode_helpers.hpp"



struct PHA_Implied_Expectations
{
    uint8_t a;
    uint8_t stack_pointer;
};

using PHAImplied     = PHA<Implied, PHA_Implied_Expectations, 3>;
using PHAImpliedMode = ParameterizedInstructionExecutorTestFixture<PHAImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const PHAImplied &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.stack_pointer = instruction_param.requirements.initial.stack_pointer;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const PHAImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const PHA_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.stack_pointer));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::PHA, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::PHA, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const PHAImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const PHAImplied                     &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer) ), Eq(instruction.requirements.final.a));
}

static const std::vector<PHAImplied> PHAImpliedModeTestValues {
PHAImplied{
    Implied().address(0x1000),
    PHAImplied::Requirements{
        .initial = {
            .a = 0,
            .stack_pointer = 0xFF
        },
        .final = {
            .a = 0,
            .stack_pointer = 0xFE
        }}
},
PHAImplied{
    Implied().address(0x10FF),
    PHAImplied::Requirements{
        .initial = {
            .a = 0xFF,
            .stack_pointer = 0x01
        },
        .final = {
            .a = 0xFF,
            .stack_pointer = 0x00
        }}
},
PHAImplied{
    Implied().address(0x1080),
    PHAImplied::Requirements{
        .initial = {
            .a = 0x7F,
            .stack_pointer = 0x00
        },
        .final = {
            .a = 0x7F,
            .stack_pointer = 0xFF
        }}
},
PHAImplied{
    Implied().address(0x1070),
    PHAImplied::Requirements{
        .initial = {
            .a = 0x80,
            .stack_pointer = 0x80
        },
        .final = {
            .a = 0x80,
            .stack_pointer = 0x7F
        }}
}
};

TEST_P(PHAImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(PushAccumulatorImpliedAtVariousAddresses,
                         PHAImpliedMode,
                         testing::ValuesIn(PHAImpliedModeTestValues) );
