#include "addressing_mode_helpers.hpp"

static inline uint8_t IgnoreCertainBitsInStatus(uint8_t input)
{
    return input | FLAGS6502::B | FLAGS6502::U; // Set this always so we can effectively ignore it during comnparisonsG
}

static inline uint8_t IgnoreBitsInStatusValueFromMemory(uint8_t input)
{
    return input | FLAGS6502::U | FLAGS6502::B | FLAGS6502::D;
}

struct PHP_Implied_Expectations
{
    uint8_t status_register;
    uint8_t stack_pointer;
};

using PHPImplied     = PHP<Implied, PHP_Implied_Expectations, 3>;
using PHPImpliedMode = ParameterizedInstructionExecutorTestFixture<PHPImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const PHPImplied &instruction_param)
{
    fixture.r.status = instruction_param.requirements.initial.status_register;
    fixture.r.stack_pointer = instruction_param.requirements.initial.stack_pointer;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const PHPImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const PHP_Implied_Expectations &expectations)
{
    uint8_t register_status = IgnoreCertainBitsInStatus(registers.status);
    uint8_t expectation_status = IgnoreCertainBitsInStatus(expectations.status_register);

    EXPECT_THAT(register_status, Eq(expectation_status));
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.stack_pointer));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::PHP, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::PHP, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const PHPImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const PHPImplied                     &instruction)
{
    // Memory ALWAYS has bit 5 set when pushed with this instruction!
    // Bit 4 will too (because it is NOT from an interrupt).
    uint8_t special_bits = FLAGS6502::B
                           | FLAGS6502::D;  // 4th bit is set when an instruction pushes on the stack.
    uint8_t memory_value = fixture.fakeMemory.at( fixture.addressUsingStackPointer(instruction.requirements.initial.stack_pointer) );

    uint8_t adjusted_memory_value                = IgnoreBitsInStatusValueFromMemory(memory_value);
    uint8_t adjusted_final_status_register_value = IgnoreBitsInStatusValueFromMemory(instruction.requirements.final.status_register);

    EXPECT_TRUE(memory_value & special_bits) << "Bits 4 & 5 are not both set in the status in stack memory";
    EXPECT_THAT(adjusted_memory_value, Eq(adjusted_final_status_register_value) ) << "The status and memory are not the same (ignoring bits 4 & 5 and 6 - D, B, and U)";
}

// NOTE: OLC emulator always sets U flag to true when executing each instruction.
static const std::vector<PHPImplied> PHPImpliedModeTestValues {
PHPImplied{
// Page ABOVE the stack (beginning)
    Implied().address(0x1000),
    PHPImplied::Requirements{
        .initial = {
            .status_register = 0,
            .stack_pointer = 0xFF
        },
        .final = {
            .status_register = 0,
            .stack_pointer = 0xFE
        }}
},

// Page ABOVE the stack (end)
PHPImplied{
    Implied().address(0x10FF),
    PHPImplied::Requirements{
        .initial = {
            .status_register = 0xFF,
            .stack_pointer = 0x01
        },
        .final = {
            .status_register = 0xFF,
            .stack_pointer = 0x00
        }}
},

// Page ABOVE the stack (middle)
PHPImplied{
    Implied().address(0x1080),
    PHPImplied::Requirements{
        .initial = {
            .status_register = 0b10101010,
            .stack_pointer = 0x00
        },
        .final = {
            .status_register = 0b10101010, // Break is cleared; U is always set
            .stack_pointer = 0xFF
        }}
},

// Somewhere else in the page
PHPImplied{
    Implied().address(0x1070),
    PHPImplied::Requirements{
        .initial = {
            .status_register = 0b01010101,
            .stack_pointer = 0x80
        },
        .final = {
            .status_register = 0b01010101,
            .stack_pointer = 0x7F
        }}
}
};

TEST_P(PHPImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(PushStatusOntoStackImpliedAtVariousAddresses,
                         PHPImpliedMode,
                         testing::ValuesIn(PHPImpliedModeTestValues) );
