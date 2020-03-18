#ifndef INSTRUCTION_HELPERS_HPP
#define INSTRUCTION_HELPERS_HPP

#include "instructions.hpp"
#include "flags.hpp"
#include <cstdint>

struct StatusExpectation
{
    constexpr StatusExpectation &flag(const FLAGS6502 f) { status_flag = f; return *this; }
    constexpr StatusExpectation &value(const bool v) { expected_value = v; return *this; }

    FLAGS6502 status_flag;
    bool      expected_value;
};

struct NZFlags
{
    StatusExpectation n_value { .status_flag = FLAGS6502::N, .expected_value = false };
    StatusExpectation z_value { .status_flag = FLAGS6502::Z, .expected_value = false };
};

struct NZCFlags
{
    StatusExpectation n_value;
    StatusExpectation z_value;
    StatusExpectation c_value;
};

struct NZRequirements
{
    NZFlags initial;
    NZFlags final;
};

struct NZCRequirements
{
    NZCFlags initial;
    NZCFlags final;
};

struct Address
{
    uint16_t instruction_address;
};

struct Absolute : Address
{
    constexpr Absolute &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr Absolute &value(uint16_t v) { absolute_address = v; return *this; }

    uint16_t absolute_address;
};

struct AbsoluteXIndexed : Absolute
{
    constexpr AbsoluteXIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr AbsoluteXIndexed &value(uint16_t v) { absolute_address = v; return *this; }
    constexpr AbsoluteXIndexed &x(uint8_t v) { x_register_value = v; return *this; }

    uint8_t x_register_value;
};

struct AbsoluteYIndexed : Absolute
{
    uint8_t y_register_value;
};

struct Immediate : Address
{
    constexpr Immediate &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr Immediate &value(uint8_t v) { immediate_value = v; return *this; }

    uint8_t immediate_value;
};

struct Implied : Address
{
};

struct Indirect : Address
{
    uint16_t indirect_address;
};

struct XIndexedIndirect : Indirect
{
    uint8_t x_register_value;
};

struct IndirectYIndexed : Indirect
{
    uint8_t y_register_value;
};

struct Relative : Address
{
    uint8_t offset;
};

struct ZeroPage : Address
{
    constexpr ZeroPage &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr ZeroPage &zp_address(uint8_t v) { zero_page_address = v; return *this; }

    uint8_t zero_page_address;
};

struct ZeroPageXIndexed : Address
{
    constexpr ZeroPageXIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr ZeroPageXIndexed &zp_address(uint8_t v) { zero_page_address = v; return *this; }

    uint8_t zero_page_address;
};

struct ZeroPageYIndexed : Address
{
    constexpr ZeroPageYIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr ZeroPageYIndexed &zp_address(uint8_t v) { zero_page_address = v; return *this; }

    uint8_t zero_page_address;
};

template<AbstractInstruction_e TOperation, typename TAddress>
struct Instruction
{
    Instruction(const TAddress &a) : address(a) { }

    TAddress              address;
    AbstractInstruction_e operation = TOperation;
};

template<typename TAddress, typename TRequirements>
struct LDA : Instruction<AbstractInstruction_e::LDA, TAddress>
{
    LDA(const TAddress &a, const TRequirements &r)
        :
        Instruction<AbstractInstruction_e::LDA, TAddress>(a),
        requirements(r)
    {
    }

    TRequirements requirements;
};

//auto instruction = LDA(Immediate(6));

#endif // INSTRUCTION_HELPERS_HPP
