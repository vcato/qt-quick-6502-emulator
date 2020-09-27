#include "addressing_mode_helpers.hpp"


void StoreOperand(InstructionExecutorTestFixture &fixture, const Absolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = fixture.loByteOf(instruction_param.absolute_address);
    fixture.fakeMemory[instruction_param.instruction_address + 2] = fixture.hiByteOf(instruction_param.absolute_address);
}

void StoreOperand(InstructionExecutorTestFixture &fixture, const IndirectYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = instruction_param.zero_page_address;
}

void StoreOperand(InstructionExecutorTestFixture &fixture, const XIndexedIndirect &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = instruction_param.zero_page_address;
}

void StoreOperand(InstructionExecutorTestFixture &fixture, const Relative &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = instruction_param.offset;
}

void StoreOperand(InstructionExecutorTestFixture &fixture, const ZeroPageXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = instruction_param.zero_page_address;
}

void StoreOperand(InstructionExecutorTestFixture &fixture, const ZeroPageYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = instruction_param.zero_page_address;
}

void StoreOperand(InstructionExecutorTestFixture &fixture, const ZeroPage &instruction_param)
{
    fixture.fakeMemory[instruction_param.instruction_address + 1] = instruction_param.zero_page_address;
}
