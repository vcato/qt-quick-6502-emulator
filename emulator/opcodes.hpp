#ifndef OPCODES_HPP
#define OPCODES_HPP

#include <cstdint>
#include "instructions.hpp"

constexpr uint8_t OpcodeFor(const AbstractInstruction_e instruction, const AddressMode_e address_mode)
{
    switch (instruction)
    {
    case AbstractInstruction_e::ADC:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0x6D;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x7D;
            break;
        case AddressMode_e::AbsoluteYIndexed:
            return 0x79;
            break;
        case AddressMode_e::Immediate:
            return 0x69;
            break;
        case AddressMode_e::XIndexedIndirect:
            return 0x61;
            break;
        case AddressMode_e::IndirectYIndexed:
            return 0x71;
            break;
        case AddressMode_e::ZeroPage:
            return 0x65;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x75;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::AND:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0x2D;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x3D;
            break;
        case AddressMode_e::AbsoluteYIndexed:
            return 0x39;
            break;
        case AddressMode_e::Immediate:
            return 0x29;
            break;
        case AddressMode_e::XIndexedIndirect:
            return 0x21;
            break;
        case AddressMode_e::IndirectYIndexed:
            return 0x31;
            break;
        case AddressMode_e::ZeroPage:
            return 0x25;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x35;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::ASL:
        switch (address_mode)
        {
        case AddressMode_e::Accumulator:
            return 0x0A;
            break;
        case AddressMode_e::Absolute:
            return 0x0E;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x1E;
            break;
        case AddressMode_e::ZeroPage:
            return 0x06;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x16;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::BCC:
        return 0x90;
        break;
    case AbstractInstruction_e::BCS:
        return 0xB0;
        break;
    case AbstractInstruction_e::BEQ:
        return 0xF0;
        break;
    case AbstractInstruction_e::BIT:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0x2C;
            break;
        case AddressMode_e::ZeroPage:
            return 0x24;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::BMI:
        return 0x30;
        break;
    case AbstractInstruction_e::BNE:
        return 0xD0;
        break;
    case AbstractInstruction_e::BPL:
        return 0x10;
        break;
    case AbstractInstruction_e::BRK:
        break;
    case AbstractInstruction_e::BVC:
        return 0x50;
        break;
    case AbstractInstruction_e::BVS:
        return 0x70;
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
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xAE;
            break;
        case AddressMode_e::AbsoluteYIndexed:
            return 0xBE;
            break;
        case AddressMode_e::Immediate:
            return 0xA2;
            break;
        case AddressMode_e::ZeroPage:
            return 0xA6;
            break;
        case AddressMode_e::ZeroPageYIndexed:
            return 0xB6;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::LDY:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xAC;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0xBC;
            break;
        case AddressMode_e::Immediate:
            return 0xA0;
            break;
        case AddressMode_e::ZeroPage:
            return 0xA4;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0xB4;
            break;
        default:
            break;
        }
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
    case AbstractInstruction_e::END:
        break;
    }
    return 0;
}

#endif // OPCODES_HPP
