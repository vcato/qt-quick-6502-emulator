#include "addressing_mode_helpers.hpp"



struct STA_XIndexedIndirect_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  x;
};

using STAXIndexedIndirect     = STA<XIndexedIndirect, STA_XIndexedIndirect_Expectations, 6>;
using STAXIndexedIndirectMode = ParameterizedInstructionExecutorTestFixture<STAXIndexedIndirect>;


void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const STAXIndexedIndirect &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to] = instruction_param.requirements.initial.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const STAXIndexedIndirect &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const STAXIndexedIndirect            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarry(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const STA_XIndexedIndirect_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::STA, XIndexedIndirect> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::STA, AddressMode_e::XIndexedIndirect) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const STAXIndexedIndirect            &instruction)
{
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t x_register    = instruction.requirements.initial.x;
    const uint8_t zero_page_address_to_load_from = fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, x_register);
    const uint8_t value         = instruction.requirements.final.a;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from    ), Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page ), Eq(value));
}


static const std::vector<STAXIndexedIndirect> STAXIndexedIndirectModeTestValues {
STAXIndexedIndirect{
    // Beginning of a page
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    STAXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .x = 12 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x7F,
            .x = 12
        }}
},
STAXIndexedIndirect{
    // The end of a page
    XIndexedIndirect().address(0x80FE).zp_address(0xA0),
    STAXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 12
        }}
},
STAXIndexedIndirect{
    // Crossing a page
    XIndexedIndirect().address(0x80FF).zp_address(0xA0),
    STAXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .x = 12 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0xFF,
            .x = 12
        }}
},
STAXIndexedIndirect{
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    STAXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 0x5F },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0,
            .x = 0x5F
        }}
},
STAXIndexedIndirect{
    XIndexedIndirect().address(0x8000).zp_address(0xA0),
    STAXIndexedIndirect::Requirements{
        .initial = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 0x60 },
        .final = {
            .address_to_indirect_to = 0xC000,
            .a = 0x80,
            .x = 0x60
        }}
}
};

TEST_P(STAXIndexedIndirectMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(StoreAccumulatorIndexedIndirectAtVariousAddresses,
                         STAXIndexedIndirectMode,
                         testing::ValuesIn(STAXIndexedIndirectModeTestValues) );
