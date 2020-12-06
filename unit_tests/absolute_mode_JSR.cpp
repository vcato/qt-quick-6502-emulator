#include "addressing_mode_helpers.hpp"



struct JSR_Absolute_Expectations
{
    uint16_t program_counter;
    uint8_t  stack_pointer;
    //uint8_t  operand;
};

using JSRAbsolute     = JSR<Absolute, JSR_Absolute_Expectations, 6>;
using JSRAbsoluteMode = ParameterizedInstructionExecutorTestFixture<JSRAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture & /* fixture */, const JSRAbsolute & /* instruction_param */)
{
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const JSRAbsolute &instruction_param)
{
    fixture.r.stack_pointer = instruction_param.requirements.initial.stack_pointer;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const JSRAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const JSR_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.program_counter, Eq(expectations.program_counter));
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.stack_pointer));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::JSR, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::JSR, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const JSRAbsolute                    &)
{
    // No computation to store
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const JSRAbsolute                    &instruction)
{
    // The stack contains the return address
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(fixture.executor.registers().stack_pointer + 1) ), Eq( fixture.loByteOf(instruction.address.instruction_address + 2) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.addressUsingStackPointer(fixture.executor.registers().stack_pointer + 2) ), Eq( fixture.hiByteOf(instruction.address.instruction_address + 2) ));
}

template<>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor &executor,
                                       const JSRAbsolute         &instruction)
{
    return executor.registers().program_counter == instruction.requirements.final.program_counter;

    // The more general relation: The program counter os loaded with the parameter of the instruction
    return executor.registers().program_counter == instruction.address.absolute_address;
}

static const std::vector<JSRAbsolute> JMPAbsoluteModeTestValues {
JSRAbsolute{
    // Beginning of a page
    Absolute().address(0x4000).value(0xA000),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x4000,
            .stack_pointer = 0xFF },
        .final = {
            .program_counter = 0xA000,
            .stack_pointer = 0xFD
        }}
},
JSRAbsolute{
    // Middle of a page
    Absolute().address(0x4080).value(0xA000),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x4080,
            .stack_pointer = 0x80 },
        .final = {
            .program_counter = 0xA000,
            .stack_pointer = 0x7E
        }}
},
JSRAbsolute{
    // End of a page
    Absolute().address(0x40FD).value(0xA000),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x40FD,
            .stack_pointer = 0x02 },
        .final = {
            .program_counter = 0xA000,
            .stack_pointer = 0x00
        }}
},
JSRAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x40FE).value(0xA0FF),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x40FE,
            .stack_pointer = 0x01 },
        .final = {
            .program_counter = 0xA0FF,
            .stack_pointer = 0xFF
        }}
},
JSRAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x40FF).value(0xA080),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x40FF,
            .stack_pointer = 0x00 },
        .final = {
            .program_counter = 0xA080,
            .stack_pointer = 0xFE
        }}
},
JSRAbsolute{
    Absolute().address(0x8000).value(0x0080),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x8000,
            .stack_pointer = 0xFF },
        .final = {
            .program_counter = 0x0080,
            .stack_pointer = 0xFD
        }}
},
JSRAbsolute{
    // Rolling TO 0
    Absolute().address(0x8000).value(0x00FF),
    JSRAbsolute::Requirements{
        .initial = {
            .program_counter = 0x8000,
            .stack_pointer = 0xFF },
        .final = {
            .program_counter = 0x00FF,
            .stack_pointer = 0xFD
        }}
}
};

TEST_P(JSRAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(JumpToAbsoluteAtVariousAddresses,
                         JSRAbsoluteMode,
                         testing::ValuesIn(JMPAbsoluteModeTestValues) );
