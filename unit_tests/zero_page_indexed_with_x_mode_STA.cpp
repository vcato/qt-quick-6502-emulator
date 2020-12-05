#include "addressing_mode_helpers.hpp"



struct STA_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
};

using STAZeroPageXIndexed     = STA<ZeroPageXIndexed, STA_ZeroPageXIndexed_Expectations, 4>;
using STAZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<STAZeroPageXIndexed>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.final.a;

}

template <>
void StoreEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture      &fixture,
                                              const ZeroPageXIndexed              &address,
                                              const STAZeroPageXIndexed::Requirements &requirements)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.x) ] = requirements.final.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAZeroPageXIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAZeroPageXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.x), Eq(instruction.requirements.initial.a));
}


static const std::vector<STAZeroPageXIndexed> STAZeroPageXIndexedModeTestValues {
STAZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x0000).zp_address(6),
    STAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 10,
            .x = 0 },
        .final = {
            .a = 10,
            .x = 0
        }}
},
STAZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x00FE).zp_address(6),
    STAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 11,
            .x = 0 },
        .final = {
            .a = 11,
            .x = 0
        }}
},
STAZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x00FF).zp_address(6),
    STAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0 },
        .final = {
            .a = 0xFF,
            .x = 0
        }}
},
// Index to the top of the page
STAZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    STAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0xEF }, // 0xEF + 0x10 = 0xFF
        .final = {
            .a = 0xFF,
            .x = 0xEF
        }}
}
#if 0
// TODO: Implement!
// Wrap-around the page
STAZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    STAZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x0F,
            .x = 0xF0 },
        .final = {
            .a = 0x0F,
            .x = 0xF0
        }}
}
#endif
};

TEST_P(STAZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorZeroPageXIndexedAtVariousAddresses,
                         STAZeroPageXIndexedMode,
                         testing::ValuesIn(STAZeroPageXIndexedModeTestValues) );
