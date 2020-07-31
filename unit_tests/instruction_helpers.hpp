#ifndef INSTRUCTION_HELPERS_HPP
#define INSTRUCTION_HELPERS_HPP

#include "instructions.hpp"
#include "flags.hpp"
#include <cstdint>

struct StatusExpectation
{
    constexpr StatusExpectation &value(const bool v) { expected_value = v; return *this; }

    bool      expected_value;
};

struct NZFlags
{
    StatusExpectation n_value { .expected_value = false };
    StatusExpectation z_value { .expected_value = false };
};

struct NZCFlags
{
    StatusExpectation n_value { .expected_value = false };
    StatusExpectation z_value { .expected_value = false };
    StatusExpectation c_value { .expected_value = false };
};

struct NZVFlags
{
    StatusExpectation n_value { .expected_value = false };
    StatusExpectation z_value { .expected_value = false };
    StatusExpectation v_value { .expected_value = false };
};

struct NZCVFlags
{
    StatusExpectation n_value { .expected_value = false };
    StatusExpectation z_value { .expected_value = false };
    StatusExpectation c_value { .expected_value = false };
    StatusExpectation v_value { .expected_value = false };
};

struct Address
{
    uint16_t instruction_address;
};

struct Accumulator : Address
{
    constexpr Accumulator &address(uint16_t a) { instruction_address = a; return *this; }

    static constexpr uint16_t operand_byte_count = 0;
    static constexpr AddressMode_e address_mode = AddressMode_e::Accumulator;
};

struct Absolute : Address
{
    constexpr Absolute &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr Absolute &value(uint16_t v) { absolute_address = v; return *this; }

    uint16_t absolute_address;

    static constexpr uint16_t operand_byte_count = 2;
    static constexpr AddressMode_e address_mode = AddressMode_e::Absolute;
};

struct AbsoluteXIndexed : Absolute
{
    constexpr AbsoluteXIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr AbsoluteXIndexed &value(uint16_t v) { absolute_address = v; return *this; }
    constexpr AbsoluteXIndexed &x(uint8_t v) { x_register_value = v; return *this; }

    uint8_t x_register_value;

    static constexpr AddressMode_e address_mode = AddressMode_e::AbsoluteXIndexed;
};

struct AbsoluteYIndexed : Absolute
{
    constexpr AbsoluteYIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr AbsoluteYIndexed &value(uint16_t v) { absolute_address = v; return *this; }
    constexpr AbsoluteYIndexed &y(uint8_t v) { y_register_value = v; return *this; }

    uint8_t y_register_value;

    static constexpr AddressMode_e address_mode = AddressMode_e::AbsoluteYIndexed;
};

struct Immediate : Address
{
    constexpr Immediate &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr Immediate &value(uint8_t v) { immediate_value = v; return *this; }

    uint8_t immediate_value;

    static constexpr uint16_t operand_byte_count = 1;
    static constexpr AddressMode_e address_mode = AddressMode_e::Immediate;
};

struct Implied : Address
{
    constexpr Implied &address(uint16_t a) { instruction_address = a; return *this; }

    static constexpr uint16_t operand_byte_count = 0;
    static constexpr AddressMode_e address_mode = AddressMode_e::Implied;
};

struct Indirect : Address
{
    uint8_t zero_page_address;

    static constexpr uint16_t operand_byte_count = 1;
    static constexpr AddressMode_e address_mode = AddressMode_e::Indirect;
};

struct XIndexedIndirect : Indirect
{
    constexpr XIndexedIndirect &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr XIndexedIndirect &zp_address(uint8_t v) { zero_page_address = v; return *this; }
    constexpr XIndexedIndirect &x(uint8_t v) { x_register_value = v; return *this; }

    uint8_t x_register_value;

    static constexpr AddressMode_e address_mode = AddressMode_e::XIndexedIndirect;
};

struct IndirectYIndexed : Indirect
{
    constexpr IndirectYIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr IndirectYIndexed &zp_address(uint8_t v) { zero_page_address = v; return *this; }
    constexpr IndirectYIndexed &y(uint8_t v) { y_register_value = v; return *this; }

    uint8_t y_register_value;

    static constexpr AddressMode_e address_mode = AddressMode_e::IndirectYIndexed;
};

struct Relative : Address
{
    constexpr Relative &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr Relative &signed_offset(uint8_t a) { offset = a; return *this; }

    uint8_t offset;

    static constexpr uint16_t operand_byte_count = 1;
    static constexpr AddressMode_e address_mode = AddressMode_e::Relative;
};

struct ZeroPage : Address
{
    constexpr ZeroPage &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr ZeroPage &zp_address(uint8_t v) { zero_page_address = v; return *this; }

    uint8_t zero_page_address;

    static constexpr uint16_t operand_byte_count = 1;
    static constexpr AddressMode_e address_mode = AddressMode_e::ZeroPage;
};

struct ZeroPageXIndexed : Address
{
    constexpr ZeroPageXIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr ZeroPageXIndexed &zp_address(uint8_t v) { zero_page_address = v; return *this; }

    uint8_t zero_page_address;

    static constexpr uint16_t operand_byte_count = 1;
    static constexpr AddressMode_e address_mode = AddressMode_e::ZeroPageXIndexed;
};

struct ZeroPageYIndexed : Address
{
    constexpr ZeroPageYIndexed &address(uint16_t a) { instruction_address = a; return *this; }
    constexpr ZeroPageYIndexed &zp_address(uint8_t v) { zero_page_address = v; return *this; }

    uint8_t zero_page_address;

    static constexpr uint16_t operand_byte_count = 1;
    static constexpr AddressMode_e address_mode = AddressMode_e::ZeroPageYIndexed;
};

template<AbstractInstruction_e TOperation, typename TAddress>
struct Instruction
{
    Instruction(const TAddress &a) : address(a) { }

    TAddress              address;
    AbstractInstruction_e operation = TOperation;
};

#endif // INSTRUCTION_HELPERS_HPP
