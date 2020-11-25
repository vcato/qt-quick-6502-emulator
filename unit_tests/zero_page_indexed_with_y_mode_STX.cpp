#include "addressing_mode_helpers.hpp"



struct STX_ZeroPageYIndexed_Expectations
{
    uint8_t x;
    uint8_t y;
};

using STXZeroPageYIndexed     = STX<ZeroPageYIndexed, STX_ZeroPageYIndexed_Expectations, 4>;
using STXZeroPageYIndexedMode = ParameterizedInstructionExecutorTestFixture<STXZeroPageYIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STXZeroPageYIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.y) ] = instruction_param.requirements.initial.x;
}

template <>
void StoreEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture      &fixture,
                                              const ZeroPageYIndexed              &address,
                                              const STXZeroPageYIndexed::Requirements &requirements)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.y) ] = requirements.initial.x;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STXZeroPageYIndexed &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.y = instruction_param.requirements.initial.y;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STXZeroPageYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STX_ZeroPageYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.y, Eq(expectations.y));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STX, ZeroPageYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STX, AddressMode_e::ZeroPageYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STXZeroPageYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.y), Eq(instruction.requirements.initial.x));
}


static const std::vector<STXZeroPageYIndexed> STXZeroPageYIndexedModeTestValues {
STXZeroPageYIndexed{
    // Beginning of a page
    ZeroPageYIndexed().address(0x0000).zp_address(6),
    STXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 10,
            .y = 0 },
        .final = {
            .x = 10,
            .y = 0
        }}
},
STXZeroPageYIndexed{
    // One before the end of a page
    ZeroPageYIndexed().address(0x00FE).zp_address(6),
    STXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 11,
            .y = 0 },
        .final = {
            .x = 11,
            .y = 0
        }}
},
STXZeroPageYIndexed{
    // Crossing a page boundary
    ZeroPageYIndexed().address(0x00FF).zp_address(6),
    STXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 32,
            .y = 0 },
        .final = {
            .x = 32,
            .y = 0
        }}
},
STXZeroPageYIndexed{
    ZeroPageYIndexed().address(0x8000).zp_address(16),
    STXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 3,
            .y = 0xEF },
        .final = {
            .x = 3,
            .y = 0xEF
        }}
}
#if 0
// These cause an extra execution cycle.
// TODO: Implement!
STXZeroPageYIndexed{
    ZeroPageYIndexed().address(0x8000).zp_address(16),
    STXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 3,
            .y = 0xF0 },
        .final = {
            .x = 3,
            .y = 0xF0
        }}
},
STXZeroPageYIndexed{
    ZeroPageYIndexed().address(0x8000).zp_address(0xFF),
    STXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .y = 0x01 },
        .final = {
            .x = 0x80,
            .y = 0x01
        }}
}
#endif
};

TEST_P(STXZeroPageYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreXZeroPageYIndexedAtVariousAddresses,
                         STXZeroPageYIndexedMode,
                         testing::ValuesIn(STXZeroPageYIndexedModeTestValues) );
