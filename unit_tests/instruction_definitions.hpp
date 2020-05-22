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

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct ADC : Instruction<AbstractInstruction_e::ADC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    ADC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::ADC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct AND : Instruction<AbstractInstruction_e::AND, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    AND(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::AND, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct ASL : Instruction<AbstractInstruction_e::ASL, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    ASL(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::ASL, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BCC : Instruction<AbstractInstruction_e::BCC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BCC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BCC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BCS : Instruction<AbstractInstruction_e::BCS, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BCS(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BCS, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BEQ : Instruction<AbstractInstruction_e::BEQ, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BEQ(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BEQ, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BIT : Instruction<AbstractInstruction_e::BIT, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BIT(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BIT, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BMI : Instruction<AbstractInstruction_e::BMI, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BMI(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BMI, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BNE : Instruction<AbstractInstruction_e::BNE, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BNE(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BNE, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BPL : Instruction<AbstractInstruction_e::BPL, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BPL(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BPL, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BRK : Instruction<AbstractInstruction_e::BRK, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BRK(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BRK, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BVC : Instruction<AbstractInstruction_e::BVC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BVC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BVC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct BVS : Instruction<AbstractInstruction_e::BVS, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    BVS(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::BVS, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CLC : Instruction<AbstractInstruction_e::CLC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CLC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CLC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CLD : Instruction<AbstractInstruction_e::CLD, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CLD(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CLD, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CLI : Instruction<AbstractInstruction_e::CLI, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CLI(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CLI, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CLV : Instruction<AbstractInstruction_e::CLV, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CLV(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CLV, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CMP : Instruction<AbstractInstruction_e::CMP, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CMP(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CMP, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CPX : Instruction<AbstractInstruction_e::CPX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CPX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CPX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct CPY : Instruction<AbstractInstruction_e::CPY, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    CPY(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::CPY, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct DEC : Instruction<AbstractInstruction_e::DEC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    DEC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::DEC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct DEX : Instruction<AbstractInstruction_e::DEX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    DEX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::DEX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct DEY : Instruction<AbstractInstruction_e::DEY, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    DEY(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::DEY, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct EOR : Instruction<AbstractInstruction_e::EOR, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    EOR(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::EOR, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct INC : Instruction<AbstractInstruction_e::INC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    INC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::INC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct INX : Instruction<AbstractInstruction_e::INX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    INX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::INX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct INY : Instruction<AbstractInstruction_e::INY, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    INY(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::INY, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct JMP : Instruction<AbstractInstruction_e::JMP, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    JMP(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::JMP, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct JSR : Instruction<AbstractInstruction_e::JSR, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    JSR(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::JSR, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct LDA : Instruction<AbstractInstruction_e::LDA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    LDA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::LDA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct LDX : Instruction<AbstractInstruction_e::LDX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    LDX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::LDX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct LDY : Instruction<AbstractInstruction_e::LDY, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    LDY(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::LDY, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct LSR : Instruction<AbstractInstruction_e::LSR, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    LSR(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::LSR, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct NOP : Instruction<AbstractInstruction_e::NOP, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    NOP(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::NOP, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct ORA : Instruction<AbstractInstruction_e::ORA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    ORA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::ORA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct PHA : Instruction<AbstractInstruction_e::PHA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    PHA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::PHA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct PHP : Instruction<AbstractInstruction_e::PHP, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    PHP(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::PHP, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct PLA : Instruction<AbstractInstruction_e::PLA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    PLA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::PLA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct PLP : Instruction<AbstractInstruction_e::PLP, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    PLP(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::PLP, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct ROL : Instruction<AbstractInstruction_e::ROL, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    ROL(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::ROL, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct ROR : Instruction<AbstractInstruction_e::ROR, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    ROR(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::ROR, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct RTI : Instruction<AbstractInstruction_e::RTI, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    RTI(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::RTI, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct RTS : Instruction<AbstractInstruction_e::RTS, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    RTS(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::RTS, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct SBC : Instruction<AbstractInstruction_e::SBC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    SBC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::SBC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct SEC : Instruction<AbstractInstruction_e::SEC, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    SEC(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::SEC, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct SED : Instruction<AbstractInstruction_e::SED, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    SED(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::SED, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct SEI : Instruction<AbstractInstruction_e::SEI, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    SEI(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::SEI, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct STA : Instruction<AbstractInstruction_e::STA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    STA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::STA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct STX : Instruction<AbstractInstruction_e::STX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    STX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::STX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct STY : Instruction<AbstractInstruction_e::STY, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    STY(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::STY, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct TAX : Instruction<AbstractInstruction_e::TAX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    TAX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::TAX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct TAY : Instruction<AbstractInstruction_e::TAY, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    TAY(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::TAY, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct TSX : Instruction<AbstractInstruction_e::TSX, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    TSX(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::TSX, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct TXA : Instruction<AbstractInstruction_e::TXA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    TXA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::TXA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct TXS : Instruction<AbstractInstruction_e::TXS, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    TXS(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::TXS, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

template<typename TAddress, class TExpectations, uint32_t TCycleCount>
struct TYA : Instruction<AbstractInstruction_e::TYA, TAddress>
{
    using Requirements = BasicRequirements<TExpectations, TCycleCount>;

    TYA(const TAddress &a, const Requirements &r)
        :
        Instruction<AbstractInstruction_e::TYA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements requirements;
};

//auto instruction = LDA(Immediate(6));

#endif // INSTRUCTION_DEFINITIONS_HPP
