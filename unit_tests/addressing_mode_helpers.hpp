#ifndef ADDRESSING_MODE_HELPERS_H
#define ADDRESSING_MODE_HELPERS_H

#include "instruction_checks.hpp"


void StoreOperand(InstructionExecutorTestFixture &fixture, const Absolute &instruction_param);
void StoreOperand(InstructionExecutorTestFixture &fixture, const IndirectYIndexed &instruction_param);
void StoreOperand(InstructionExecutorTestFixture &fixture, const XIndexedIndirect &instruction_param);
void StoreOperand(InstructionExecutorTestFixture &fixture, const Relative &instruction_param);
void StoreOperand(InstructionExecutorTestFixture &fixture, const ZeroPageXIndexed &instruction_param);
void StoreOperand(InstructionExecutorTestFixture &fixture, const ZeroPageYIndexed &instruction_param);
void StoreOperand(InstructionExecutorTestFixture &fixture, const ZeroPage &instruction_param);

template <typename TInstructionParamAddress, typename TInstructionParamRequirements>
void StoreEffectiveAddress(InstructionExecutorTestFixture      &fixture,
                           const TInstructionParamAddress      &address,
                           const TInstructionParamRequirements &requirements)
{
    fixture.fakeMemory[address.zero_page_address    ]   = fixture.loByteOf(requirements.initial.address_to_indirect_to);
    fixture.fakeMemory[address.zero_page_address + 1]   = fixture.hiByteOf(requirements.initial.address_to_indirect_to);
}

template <typename TInstructionParamAddress, typename TInstructionParamRequirements>
void StoreEffectiveAddressWithNoCarry(InstructionExecutorTestFixture      &fixture,
                                      const TInstructionParamAddress      &address,
                                      const TInstructionParamRequirements &requirements)
{
    auto effective_address = fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.x);

    fixture.fakeMemory[ effective_address     ] = fixture.loByteOf(requirements.initial.address_to_indirect_to);
    fixture.fakeMemory[ effective_address + 1 ] = fixture.hiByteOf(requirements.initial.address_to_indirect_to);
}

template <typename TInstructionParamAddress, typename TInstructionParamRequirements>
void StoreEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture      &fixture,
                                              const TInstructionParamAddress      &address,
                                              const TInstructionParamRequirements &requirements)
{
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(address.zero_page_address, requirements.initial.x) ] = requirements.initial.operand;
}

void StoreTestValueAtImmediateAddress(InstructionExecutorTestFixture &fixture, const Immediate &instruction_param);
void StoreTestValueRelativeAddress(InstructionExecutorTestFixture &fixture, const Relative &instruction_param);


template <typename InstructionParam>
void StoreOpcode(InstructionExecutorTestFixture &fixture, const InstructionParam &instruction_param)
{
    fixture.loadOpcodeIntoMemory(
        instruction_param.operation,
        instruction_param.address.address_mode,
        instruction_param.address.instruction_address
        );
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveAnEffectiveAddress(InstructionExecutorTestFixture &fixture,
                                                       const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreOperand(fixture, instruction_param.address);
    StoreTestValueAtEffectiveAddress(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddress(InstructionExecutorTestFixture &fixture,
                                                                 const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreOperand(fixture, instruction_param.address);
    StoreEffectiveAddress(fixture, instruction_param.address, instruction_param.requirements);
    StoreTestValueAtEffectiveAddress(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarry(InstructionExecutorTestFixture &fixture,
                                                                            const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreOperand(fixture, instruction_param.address);
    StoreEffectiveAddressWithNoCarry(fixture, instruction_param.address, instruction_param.requirements);
    StoreTestValueAtEffectiveAddress(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddressWithNoCarryZeroPage(InstructionExecutorTestFixture &fixture,
                                                                                    const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreOperand(fixture, instruction_param.address);
    StoreEffectiveAddressWithNoCarryZeroPage(fixture, instruction_param.address, instruction_param.requirements);
    StoreTestValueAtEffectiveAddress(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveNoEffectiveAddress(InstructionExecutorTestFixture &fixture,
                                                       const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveImmediateValue(InstructionExecutorTestFixture &fixture,
                                                   const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreTestValueAtImmediateAddress(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveRelativeValue(InstructionExecutorTestFixture &fixture,
                                                   const InstructionParam        &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
    StoreTestValueRelativeAddress(fixture, instruction_param);
}

template <typename InstructionParam>
void SetupRAMForInstructionsThatHaveImpliedValue(InstructionExecutorTestFixture &fixture,
                                                 const InstructionParam         &instruction_param)
{
    StoreOpcode(fixture, instruction_param);
}

#endif // ADDRESSING_MODE_HELPERS_H
