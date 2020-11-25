#include "addressing_mode_helpers.hpp"



struct STA_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
};

using STAIndirectYIndexed     = STA<IndirectYIndexed, STA_IndirectYIndexed_Expectations, 6>;
using STAIndirectYIndexedMode = ParameterizedInstructionExecutorTestFixture<STAIndirectYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAIndirectYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAIndirectYIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAIndirectYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_IndirectYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAIndirectYIndexed            &instruction)
{
    const uint8_t zero_page_address_to_load_from = instruction.address.zero_page_address;
    const uint8_t value_to_load                  = instruction.requirements.initial.a;
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t y_register                     = instruction.requirements.initial.y;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from ),    Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page + y_register ), Eq(value_to_load));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const STAIndirectYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.requirements.initial.address_to_indirect_to);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = (fixture.hiByteOf(effective_address) - fixture.hiByteOf(original_address)) > 0x00;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<STAIndirectYIndexed> STAIndirectYIndexedModeTestValues {
STAIndirectYIndexed{
    // Beginning of a page
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    STAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .y = 12
        }}
},
STAIndirectYIndexed{
    // End of a page
    IndirectYIndexed().address(0x10FE).zp_address(0xA0),
    STAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 12 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .y = 12
        }}
},
STAIndirectYIndexed{
    // Crossing a page boundary
    IndirectYIndexed().address(0x10FF).zp_address(0xA0),
    STAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .y = 12 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .y = 12
        }}
},
STAIndirectYIndexed{
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    STAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0x5F }, // Puts us at end-of-page
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .y = 0x5F
        }}
},
STAIndirectYIndexed{
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    STAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .y = 0x60 }, // What happens when we go over 0xFF in the page?
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .y = 0x60
        }}
},
STAIndirectYIndexed{
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    STAIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .y = 0xFF },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 1,
            .y = 0xFF
        }}
}
};

TEST_P(STAIndirectYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorIndirectYIndexedAtVariousAddresses,
                         STAIndirectYIndexedMode,
                         testing::ValuesIn(STAIndirectYIndexedModeTestValues) );
