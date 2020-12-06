#include "addressing_mode_helpers.hpp"



struct NOP_Implied_Expectations
{
};

using NOPImplied     = NOP<Implied, NOP_Implied_Expectations, 2>;
using NOPImpliedMode = ParameterizedInstructionExecutorTestFixture<NOPImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &, const NOPImplied &)
{
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const NOPImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveNoEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &,
                                 const NOP_Implied_Expectations &)
{
    // No registers affected  (PC is tested elsewhere)
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::NOP, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::NOP, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const NOPImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const NOPImplied                     &)
{
    // No memory affected
}

static const std::vector<NOPImplied> NOPImpliedModeTestValues {
NOPImplied{
    Implied().address(0x1000),
    NOPImplied::Requirements{
        .initial = { },
        .final = { }}
},
NOPImplied{
    Implied().address(0x10FF),
    NOPImplied::Requirements{
        .initial = { },
        .final = { }}
},
NOPImplied{
    Implied().address(0x1080),
    NOPImplied::Requirements{
        .initial = { },
        .final = { }}
},
NOPImplied{
    Implied().address(0x1070),
    NOPImplied::Requirements{
        .initial = { },
        .final = { }}
}
};

TEST_P(NOPImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(NoOperationImpliedAtVariousAddresses,
                         NOPImpliedMode,
                         testing::ValuesIn(NOPImpliedModeTestValues) );
