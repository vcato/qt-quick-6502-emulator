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
        return 0x18;
        break;
    case AbstractInstruction_e::CLD:
        return 0xD8;
        break;
    case AbstractInstruction_e::CLI:
        return 0x58;
        break;
    case AbstractInstruction_e::CLV:
        return 0xB8;
        break;
    case AbstractInstruction_e::CMP:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xCD;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0xDD;
            break;
        case AddressMode_e::AbsoluteYIndexed:
            return 0xD9;
            break;
        case AddressMode_e::Immediate:
            return 0xC9;
            break;
        case AddressMode_e::XIndexedIndirect:
            return 0xC1;
            break;
        case AddressMode_e::IndirectYIndexed:
            return 0xD1;
            break;
        case AddressMode_e::ZeroPage:
            return 0xC5;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0xD5;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::CPX:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xEC;
            break;
        case AddressMode_e::Immediate:
            return 0xE0;
            break;
        case AddressMode_e::ZeroPage:
            return 0xE4;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::CPY:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xCC;
            break;
        case AddressMode_e::Immediate:
            return 0xC0;
            break;
        case AddressMode_e::ZeroPage:
            return 0xC4;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::DEC:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xCE;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0xDE;
            break;
        case AddressMode_e::ZeroPage:
            return 0xC6;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0xD6;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::DEX:
        return 0xCA;
        break;
    case AbstractInstruction_e::DEY:
        return 0x88;
        break;
    case AbstractInstruction_e::EOR:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0x4D;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x5D;
            break;
        case AddressMode_e::AbsoluteYIndexed:
            return 0x59;
            break;
        case AddressMode_e::Immediate:
            return 0x49;
            break;
        case AddressMode_e::XIndexedIndirect:
            return 0x41;
            break;
        case AddressMode_e::IndirectYIndexed:
            return 0x51;
            break;
        case AddressMode_e::ZeroPage:
            return 0x45;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x55;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::INC:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0xEE;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0xFE;
            break;
        case AddressMode_e::ZeroPage:
            return 0xE6;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0xF6;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::INX:
        return 0xE8;
        break;
    case AbstractInstruction_e::INY:
        return 0xC8;
        break;
    case AbstractInstruction_e::JMP:
        switch (address_mode)
        {
        case AddressMode_e::Absolute:
            return 0x4C;
            break;
        case AddressMode_e::Indirect:
            return 0x6C;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::JSR:
        return 0x20;
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
        switch (address_mode)
        {
        case AddressMode_e::Accumulator:
            return 0x4A;
            break;
        case AddressMode_e::Absolute:
            return 0x4E;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x5E;
            break;
        case AddressMode_e::ZeroPage:
            return 0x46;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x56;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::NOP:
        return 0xEA;
        break;
    case AbstractInstruction_e::ORA:
        break;
    case AbstractInstruction_e::PHA:
        return 0x48;
        break;
    case AbstractInstruction_e::PHP:
        return 0x08;
        break;
    case AbstractInstruction_e::PLA:
        return 0x68;
        break;
    case AbstractInstruction_e::PLP:
        return 0x28;
        break;
    case AbstractInstruction_e::ROL:
        switch (address_mode)
        {
        case AddressMode_e::Accumulator:
            return 0x2A;
            break;
        case AddressMode_e::Absolute:
            return 0x2E;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x3E;
            break;
        case AddressMode_e::ZeroPage:
            return 0x26;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x36;
            break;
        default:
            break;
        }
        break;
    case AbstractInstruction_e::ROR:
        switch (address_mode)
        {
        case AddressMode_e::Accumulator:
            return 0x6A;
            break;
        case AddressMode_e::Absolute:
            return 0x6E;
            break;
        case AddressMode_e::AbsoluteXIndexed:
            return 0x7E;
            break;
        case AddressMode_e::ZeroPage:
            return 0x66;
            break;
        case AddressMode_e::ZeroPageXIndexed:
            return 0x76;
            break;
        default:
            break;
        }
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
