#include "addressing_mode_helpers.hpp"



struct STY_ZeroPageXIndexed_Expectations
{
    uint8_t y;
    uint8_t x;
};

using STYZeroPageXIndexed     = STY<ZeroPageXIndexed, STY_ZeroPageXIndexed_Expectations, 4>;
using STYZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<STYZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STYZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.y;

}

template <>
void StoreEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture      &fixture,
                                              const ZeroPageXIndexed              &address,
                                              const STYZeroPageXIndexed::Requirements &requirements)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.x) ] = requirements.initial.y;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STYZeroPageXIndexed &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.x = instruction_param.requirements.initial.x;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STYZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STY_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.x, Eq(expectations.x));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STY, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STY, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STYZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.initial.y));
}


static const std::vector<STYZeroPageXIndexed> STYZeroPageXIndexedModeTestValues {
STYZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x0000).zp_address(6),
    STYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 10,
            .x = 0 },
        .final = {
            .y = 10,
            .x = 0
        }}
},
STYZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x00FE).zp_address(6),
    STYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 11,
            .x = 0 },
        .final = {
            .y = 11,
            .x = 0
        }}
},
STYZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x00FF).zp_address(6),
    STYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0xFF,
            .x = 0 },
        .final = {
            .y = 0xFF,
            .x = 0
        }}
},
// Index to the top of the page
STYZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    STYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0xFF,
            .x = 0xEF }, // 0xEF + 0x10 = 0xFF
        .final = {
            .y = 0xFF,
            .x = 0xEF
        }}
},
// Wrap-around the page`
#if 0
// TODO: Implement counting the extra-cycle
STYZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    STYZeroPageXIndexed::Requirements{
        .initial = {
            .y = 0x0F,
            .x = 0xF0 }, // 0xF0 + 0x10 = 0x00
        .final = {
            .y = 0x0F,
            .x = 0xF0
        }}
}
#endif
};

TEST_P(STYZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreYZeroPageXIndexedAtVariousAddresses,
                         STYZeroPageXIndexedMode,
                         testing::ValuesIn(STYZeroPageXIndexedModeTestValues) );
