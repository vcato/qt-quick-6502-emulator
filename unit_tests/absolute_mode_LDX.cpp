#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDX_Absolute_Expectations
{
    uint8_t x;
    NZFlags flags;
};

using LDXAbsolute     = LDX<Absolute, LDX_Absolute_Expectations, 4>;
using LDXAbsoluteMode = ParameterizedInstructionExecutorTestFixture<LDXAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDXAbsolute                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Absolute,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.final.x;

    // Load appropriate registers
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDX_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDX, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDX, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDXAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.final.x ));
}


static const std::vector<LDXAbsolute> LDXAbsoluteModeTestValues {
LDXAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXAbsolute{
    // Loading a zero affects the Z flag
    Absolute().address(0x8000).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0xA0,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDXAbsolute{
    // Loading a negative affects the N flag
    Absolute().address(0x8000).value(0xA000),
    LDXAbsolute::Requirements{
        .initial = {
            .x = 0x10,
            .flags = { }},
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDXAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteAtVariousAddresses,
                         LDXAbsoluteMode,
                         testing::ValuesIn(LDXAbsoluteModeTestValues) );
