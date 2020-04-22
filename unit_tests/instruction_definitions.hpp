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

//auto instruction = LDA(Immediate(6));

#endif // INSTRUCTION_DEFINITIONS_HPP
