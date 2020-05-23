#ifndef INSTRUCTION_DEFINITIONS_HPP
#define INSTRUCTION_DEFINITIONS_HPP

#include "instruction_helpers.hpp"

template<class TExpectations, uint32_t TCycleCount>
struct BasicRequirements
{
    TExpectations initial;
    TExpectations final;
    static constexpr auto cycle_count = TCycleCount;
};

template<AbstractInstruction_e TOperation, typename TAddress, class TExpectations, uint32_t TCycleCount>
struct InstructionWithRequirements : Instruction<TOperation, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    InstructionWithRequirements(const TAddress &a, const Requirements &r)
        :
        Instruction<TOperation, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using ADC = InstructionWithRequirements<AbstractInstruction_e::ADC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using AND = InstructionWithRequirements<AbstractInstruction_e::AND, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using ASL = InstructionWithRequirements<AbstractInstruction_e::ASL, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BCC = InstructionWithRequirements<AbstractInstruction_e::BCC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BCS = InstructionWithRequirements<AbstractInstruction_e::BCS, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BEQ = InstructionWithRequirements<AbstractInstruction_e::BEQ, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BIT = InstructionWithRequirements<AbstractInstruction_e::BIT, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BMI = InstructionWithRequirements<AbstractInstruction_e::BMI, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BNE = InstructionWithRequirements<AbstractInstruction_e::BNE, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BPL = InstructionWithRequirements<AbstractInstruction_e::BPL, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BRK = InstructionWithRequirements<AbstractInstruction_e::BRK, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BVC = InstructionWithRequirements<AbstractInstruction_e::BVC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using BVS = InstructionWithRequirements<AbstractInstruction_e::BVS, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CLC = InstructionWithRequirements<AbstractInstruction_e::CLC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CLD = InstructionWithRequirements<AbstractInstruction_e::CLD, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CLI = InstructionWithRequirements<AbstractInstruction_e::CLI, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CLV = InstructionWithRequirements<AbstractInstruction_e::CLV, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CMP = InstructionWithRequirements<AbstractInstruction_e::CMP, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CPX = InstructionWithRequirements<AbstractInstruction_e::CPX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using CPY = InstructionWithRequirements<AbstractInstruction_e::CPY, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using DEC = InstructionWithRequirements<AbstractInstruction_e::DEC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using DEX = InstructionWithRequirements<AbstractInstruction_e::DEX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using DEY = InstructionWithRequirements<AbstractInstruction_e::DEY, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using EOR = InstructionWithRequirements<AbstractInstruction_e::EOR, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using INC = InstructionWithRequirements<AbstractInstruction_e::INC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using INX = InstructionWithRequirements<AbstractInstruction_e::INX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using INY = InstructionWithRequirements<AbstractInstruction_e::INY, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using JMP = InstructionWithRequirements<AbstractInstruction_e::JMP, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using JSR = InstructionWithRequirements<AbstractInstruction_e::JSR, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using LDA = InstructionWithRequirements<AbstractInstruction_e::LDA, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using LDX = InstructionWithRequirements<AbstractInstruction_e::LDX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using LDY = InstructionWithRequirements<AbstractInstruction_e::LDY, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using LSR = InstructionWithRequirements<AbstractInstruction_e::LSR, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using NOP = InstructionWithRequirements<AbstractInstruction_e::NOP, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using ORA = InstructionWithRequirements<AbstractInstruction_e::ORA, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using PHA = InstructionWithRequirements<AbstractInstruction_e::PHA, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using PHP = InstructionWithRequirements<AbstractInstruction_e::PHP, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using PLA = InstructionWithRequirements<AbstractInstruction_e::PLA, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using PLP = InstructionWithRequirements<AbstractInstruction_e::PLP, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using ROL = InstructionWithRequirements<AbstractInstruction_e::ROL, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using ROR = InstructionWithRequirements<AbstractInstruction_e::ROR, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using RTI = InstructionWithRequirements<AbstractInstruction_e::RTI, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using RTS = InstructionWithRequirements<AbstractInstruction_e::RTS, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using SBC = InstructionWithRequirements<AbstractInstruction_e::SBC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using SEC = InstructionWithRequirements<AbstractInstruction_e::SEC, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using SED = InstructionWithRequirements<AbstractInstruction_e::SED, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using SEI = InstructionWithRequirements<AbstractInstruction_e::SEI, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using STA = InstructionWithRequirements<AbstractInstruction_e::STA, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using STX = InstructionWithRequirements<AbstractInstruction_e::STX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using STY = InstructionWithRequirements<AbstractInstruction_e::STY, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using TAX = InstructionWithRequirements<AbstractInstruction_e::TAX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using TAY = InstructionWithRequirements<AbstractInstruction_e::TAY, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using TSX = InstructionWithRequirements<AbstractInstruction_e::TSX, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using TXA = InstructionWithRequirements<AbstractInstruction_e::TXA, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using TXS = InstructionWithRequirements<AbstractInstruction_e::TXS, TAddress, TExpectations, TCycleCount>;

template<typename TAddress, typename TExpectations, uint32_t TCycleCount>
using TYA = InstructionWithRequirements<AbstractInstruction_e::TYA, TAddress, TExpectations, TCycleCount>;

//auto instruction = LDA(Immediate(6));

#endif // INSTRUCTION_DEFINITIONS_HPP
