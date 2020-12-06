#include "addressing_mode_helpers.hpp"



struct JMP_Absolute_Expectations
{
    uint16_t program_counter;
};

using JMPAbsolute     = JMP<Absolute, JMP_Absolute_Expectations, 3>;
using JMPAbsoluteMode = ParameterizedInstructionExecutorTestFixture<JMPAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture & /* fixture */, const JMPAbsolute & /* instruction_param */)
{
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture & /* fixture */, const JMPAbsolute & /* instruction_param */)
{
    // Load appropriate registers
    // None to load...
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const JMPAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const JMP_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::JMP, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::JMP, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const JMPAbsolute                    &)
{
    // No memory changes
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const JMPAbsolute                    &)
{
    // No memory stores the result
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor &executor,
                                       const JMPAbsolute         &instruction)
{
    // One past the instruction, for typical instructions.
    return executor.registers().program_counter == instruction.requirements.final.program_counter;
}

static const std::vector<JMPAbsolute> JMPAbsoluteModeTestValues {
JMPAbsolute{
    // Beginning of a page
    Absolute().address(0x4000).value(0xA000),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x4000 },
        .final = {
            .program_counter = 0xA000
        }}
},
JMPAbsolute{
    // Middle of a page
    Absolute().address(0x4080).value(0xA000),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x4080 },
        .final = {
            .program_counter = 0xA000
        }}
},
JMPAbsolute{
    // End of a page
    Absolute().address(0x40FD).value(0xA000),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x40FD },
        .final = {
            .program_counter = 0xA000
        }}
},
JMPAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x40FE).value(0xA0FF),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x40FE },
        .final = {
            .program_counter = 0xA0FF
        }}
},
JMPAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x40FF).value(0xA080),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x40FF },
        .final = {
            .program_counter = 0xA080
        }}
},
JMPAbsolute{
    Absolute().address(0x8000).value(0x0080),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x8000 },
        .final = {
            .program_counter = 0x0080
        }}
},
JMPAbsolute{
    // Rolling TO 0
    Absolute().address(0x8000).value(0x00FF),
    JMPAbsolute::Requirements{
        .initial = {
            .program_counter = 0x8000 },
        .final = {
            .program_counter = 0x00FF
        }}
}
};

TEST_P(JMPAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(JumpToAbsoluteAtVariousAddresses,
                         JMPAbsoluteMode,
                         testing::ValuesIn(JMPAbsoluteModeTestValues) );
