#include "addressing_mode_helpers.hpp"



struct ASL_Absolute_Expectations
{
    constexpr ASL_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t  a;
    NZCFlags flags;

    uint8_t   operand;
};

using ASLAbsolute     = ASL<Absolute, ASL_Absolute_Expectations, 6>;
using ASLAbsoluteMode = ParameterizedInstructionExecutorTestFixture<ASLAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ASLAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ASLAbsolute &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ASLAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ASL_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ASL, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ASL, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ASLAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const ASLAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq(instruction.requirements.final.operand));
}

static const std::vector<ASLAbsolute> ASLAbsoluteModeTestValues {
ASLAbsolute{
    // Beginning of a page
    Absolute().address(0x4000).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsolute{
    // Middle of a page
    Absolute().address(0x4088).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsolute{
    // End of a page
    Absolute().address(0x40FD).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x40FE).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x40FF).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsolute{
    // Check for High bit going into carry
    Absolute().address(0x8000).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 1,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01010100
        }}
},
ASLAbsolute{
    // Check for N flag
    Absolute().address(0x8000).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0b11101010 },
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b11010100
        }}
},
ASLAbsolute{
    // Zero is set in lowest bit
    Absolute().address(0x8000).value(0xA000),
    ASLAbsolute::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { },
            .operand = 0b00000001 },
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010
        }}
}
};

TEST_P(ASLAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ArithmeticShiftLeftAbsoluteAtVariousAddresses,
                         ASLAbsoluteMode,
                         testing::ValuesIn(ASLAbsoluteModeTestValues) );
