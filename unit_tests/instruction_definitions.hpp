#ifndef INSTRUCTION_DEFINITIONS_HPP
#define INSTRUCTION_DEFINITIONS_HPP

#include "instruction_helpers.hpp"

template<class TExpectations>
struct Requirements
{
    TExpectations initial;
    TExpectations final;
};

template<typename TAddress, class TExpectations>
struct LDA : Instruction<AbstractInstruction_e::LDA, TAddress>
{
    LDA(const TAddress &a, const Requirements<TExpectations> &r)
        :
        Instruction<AbstractInstruction_e::LDA, TAddress>(a),
        requirements(r)
    {
    }

    Requirements<TExpectations> requirements;
};

//auto instruction = LDA(Immediate(6));

#endif // INSTRUCTION_DEFINITIONS_HPP
