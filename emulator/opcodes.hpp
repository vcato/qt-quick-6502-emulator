#ifndef OPCODES_HPP
#define OPCODES_HPP

#include <cstdint>
#include "instructions.hpp"

constexpr uint8_t OpcodeFor(const AbstractInstruction_e instruction, const AddressMode_e address_mode)
{
    switch (instruction)
    {
    case AbstractInstruction_e::ADC:
        break;
    case AbstractInstruction_e::AND:
        break;
    case AbstractInstruction_e::ASL:
        break;
    case AbstractInstruction_e::BCC:
        break;
    case AbstractInstruction_e::BCS:
        break;
    case AbstractInstruction_e::BEQ:
        break;
    case AbstractInstruction_e::BIT:
        break;
    case AbstractInstruction_e::BMI:
        break;
    case AbstractInstruction_e::BNE:
        break;
    case AbstractInstruction_e::BPL:
        break;
    case AbstractInstruction_e::BRK:
        break;
    case AbstractInstruction_e::BVC:
        break;
    case AbstractInstruction_e::BVS:
        break;
    case AbstractInstruction_e::CLC:
        break;
    case AbstractInstruction_e::CLD:
        break;
    case AbstractInstruction_e::CLI:
        break;
    case AbstractInstruction_e::CLV:
        break;
    case AbstractInstruction_e::CMP:
        break;
    case AbstractInstruction_e::CPX:
        break;
    case AbstractInstruction_e::CPY:
        break;
    case AbstractInstruction_e::DEC:
        break;
    case AbstractInstruction_e::DEX:
        break;
    case AbstractInstruction_e::DEY:
        break;
    case AbstractInstruction_e::EOR:
        break;
    case AbstractInstruction_e::INC:
        break;
    case AbstractInstruction_e::INX:
        break;
    case AbstractInstruction_e::INY:
        break;
    case AbstractInstruction_e::JMP:
        break;
    case AbstractInstruction_e::JSR:
        break;
    case AbstractInstruction_e::LDA:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xAD;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0xBD;
            break;
        case AddressMode_e::AbsoluteYIndexed:
            return 0xB9;
            break;
        case AddressMode_e::Immediate:
            return 0xA9;
            break;
        case AddressMode_e::XIndexedIndirect:
            return 0xA1;
            break;
        case AddressMode_e::IndirectYIndexed:
            return 0xB1;
            break;
        case AddressMode_e::ZeroPage:
            return 0xA5;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0xB5;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::LDX:
        break;
    case AbstractInstruction_e::LDY:
        break;
    case AbstractInstruction_e::LSR:
        break;
    case AbstractInstruction_e::NOP:
        break;
    case AbstractInstruction_e::ORA:
        break;
    case AbstractInstruction_e::PHA:
        break;
    case AbstractInstruction_e::PHP:
        break;
    case AbstractInstruction_e::PLA:
        break;
    case AbstractInstruction_e::PLP:
        break;
    case AbstractInstruction_e::ROL:
        break;
    case AbstractInstruction_e::ROR:
        break;
    case AbstractInstruction_e::RTI:
        break;
    case AbstractInstruction_e::RTS:
        break;
    case AbstractInstruction_e::SBC:
        break;
    case AbstractInstruction_e::SEC:
        break;
    case AbstractInstruction_e::SED:
        break;
    case AbstractInstruction_e::SEI:
        break;
    case AbstractInstruction_e::STA:
        break;
    case AbstractInstruction_e::STX:
        break;
    case AbstractInstruction_e::STY:
        break;
    case AbstractInstruction_e::TAX:
        break;
    case AbstractInstruction_e::TAY:
        break;
    case AbstractInstruction_e::TSX:
        break;
    case AbstractInstruction_e::TXA:
        break;
    case AbstractInstruction_e::TXS:
        break;
    case AbstractInstruction_e::TYA:
        break;
    }
    return 0;
}

#endif // OPCODES_HPP
