#ifndef CPU_HPP
#define CPU_HPP

#include <functional>

#define INSTRUCTION_EXECUTOR 0

/*
    olc6502 - An emulation of the 6502/2A03 processor
    "Thanks Dad for believing computers were gonna be a big deal..." - javidx9

    License (OLC-3)
    ~~~~~~~~~~~~~~~

    Copyright 2018-2019 OneLoneCoder.com

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions or derivations of source code must retain the above
    copyright notice, this list of conditions and the following disclaimer.

    2. Redistributions or derivative works in binary form must reproduce
    the above copyright notice. This list of conditions and the following
    disclaimer must be reproduced in the documentation and/or other
    materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Background
    ~~~~~~~~~~
    I love this microprocessor. It was at the heart of two of my favourite
    machines, the BBC Micro, and the Nintendo Entertainment System, as well
    as countless others in that era. I learnt to program on the Model B, and
    I learnt to love games on the NES, so in many ways, this processor is
    why I am the way I am today.

    In February 2019, I decided to undertake a selfish personal project and
    build a NES emulator. Ive always wanted to, and as such I've avoided
    looking at source code for such things. This made making this a real
    personal challenge. I know its been done countless times, and very likely
    in far more clever and accurate ways than mine, but I'm proud of this.

    Datasheet: http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf

    Files: olc6502.h, olc6502.cpp

    Relevant Video: https://youtu.be/8XmxKPJDGU0

    Links
    ~~~~~
    YouTube:	https://www.youtube.com/javidx9
                https://www.youtube.com/javidx9extra
    Discord:	https://discord.gg/WhwHUMV
    Twitter:	https://www.twitter.com/javidx9
    Twitch:		https://www.twitch.tv/javidx9
    GitHub:		https://www.github.com/onelonecoder
    Patreon:	https://www.patreon.com/javidx9
    Homepage:	https://www.onelonecoder.com

    Author
    ~~~~~~
    David Barr, aka javidx9, ©OneLoneCoder 2019
*/

#include <QObject>
#include <QPointer>
#include <string>
#include <map>
#include "registers.hpp"
#if INSTRUCTION_EXECUTOR
#include "instructionexecutor.hpp"
#endif


class olc6502 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int a            READ property_a      NOTIFY aChanged)
    Q_PROPERTY(int x            READ property_x      NOTIFY xChanged)
    Q_PROPERTY(int y            READ property_y      NOTIFY yChanged)
    Q_PROPERTY(int stackPointer READ property_stkp   NOTIFY stackPointerChanged)
    Q_PROPERTY(int pc           READ property_pc     NOTIFY pcChanged)
    Q_PROPERTY(int status       READ property_status NOTIFY statusChanged)

    Q_PROPERTY(bool log         READ log             WRITE setLog NOTIFY logChanged)
public:
    using addressType = uint16_t;
    using disassemblyType = std::map<addressType, std::string>;

    Q_ENUM(FLAGS6502)

    explicit olc6502(QObject *parent = nullptr);

    static void RegisterType();

#if !INSTRUCTION_EXECUTOR
    // Addressing Modes =============================================
    // The 6502 has a variety of addressing modes to access data in
    // memory, some of which are direct and some are indirect (like
    // pointers in C++). Each opcode contains information about which
    // addressing mode should be employed to facilitate the
    // instruction, in regards to where it reads/writes the data it
    // uses. The address mode changes the number of bytes that
    // makes up the full instruction, so we implement addressing
    // before executing the instruction, to make sure the program
    // counter is at the correct location, the instruction is
    // primed with the addresses it needs, and the number of clock
    // cycles the instruction requires is calculated. These functions
    // may adjust the number of cycles required depending upon where
    // and how the memory is accessed, so they return the required
    // adjustment.

    uint8_t IMP(); uint8_t IMM();
    uint8_t ZP0(); uint8_t ZPX();
    uint8_t ZPY(); uint8_t REL();
    uint8_t ABS(); uint8_t ABX();
    uint8_t ABY(); uint8_t IND();
    uint8_t IZX(); uint8_t IZY();

    // Opcodes ======================================================
    // There are 56 "legitimate" opcodes provided by the 6502 CPU. I
    // have not modelled "unofficial" opcodes. As each opcode is
    // defined by 1 byte, there are potentially 256 possible codes.
    // Codes are not used in a "switch case" style on a processor,
    // instead they are repsonisble for switching individual parts of
    // CPU circuits on and off. The opcodes listed here are official,
    // meaning that the functionality of the chip when provided with
    // these codes is as the developers intended it to be. Unofficial
    // codes will of course also influence the CPU circuitry in
    // interesting ways, and can be exploited to gain additional
    // functionality!
    //
    // These functions return 0 normally, but some are capable of
    // requiring more clock cycles when executed under certain
    // conditions combined with certain addressing modes. If that is
    // the case, they return 1.
    //
    // I have included detailed explanations of each function in
    // the class implementation file. Note they are listed in
    // alphabetical order here for ease of finding.

    uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
    uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
    uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
    uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
    uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
    uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
    uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
    uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
    uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
    uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
    uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
    uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
    uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
    uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

    // I capture all "unofficial" opcodes with this function. It is
    // functionally identical to a NOP
    uint8_t XXX();
#endif

    void reset();

    void irq();
    void nmi();

#if !INSTRUCTION_EXECUTOR
    // The read location of data can come from two sources, a memory address, or
    // its immediately available as part of the instruction. This function decides
    // depending on address mode of instruction byte
    uint8_t fetch();
#endif

    // Indicates the current instruction has completed by returning true. This is
    // a utility function to enable "step-by-step" execution, without manually
    // clocking every cycle
    bool complete() const;

    uint8_t a() const { return _registers.a; }
    uint8_t x() const { return _registers.x; }
    uint8_t y() const { return _registers.y; }

    uint16_t pc() const { return _registers.program_counter; }
    uint8_t  stackPointer() const { return _registers.stack_pointer; }
    uint8_t  status() const { return _registers.status; }

    const Registers &registers() const { return _registers; }
          Registers &registers()       { return _registers; }

#if INSTRUCTION_EXECUTOR
    uint32_t clockTicks() const { return _executor.clockTicks(); }
#else
    uint32_t clockTicks() const { return _clock_count; }
#endif

    bool log() const { return _log; }
    void setLog(bool value);

    auto disassemble(addressType start, addressType stop) -> disassemblyType;
public slots:
    void clock(); ///< Executes one clock tick

    uint8_t read(addressType address, bool read_only = false);
    void    write(addressType address, uint8_t data);

signals:
    uint8_t readSignal(addressType address, bool read_only);
    void    writeSignal(addressType address, uint8_t data);

    void aChanged(uint8_t new_value);
    void xChanged(uint8_t new_value);
    void yChanged(uint8_t new_value);
    void stackPointerChanged(uint8_t new_value);
    void pcChanged(uint16_t new_value);
    void statusChanged(uint8_t new_value);

    void logChanged();

private:
#if !INSTRUCTION_EXECUTOR
    // This structure and the following vector are used to compile and store
    // the opcode translation table. The 6502 can effectively have 256
    // different instructions. Each of these are stored in a table in numerical
    // order so they can be looked up easily, with no decoding required.
    // Each table entry holds:
    //	Pneumonic : A textual representation of the instruction (used for disassembly)
    //	Opcode Function: A function pointer to the implementation of the opcode
    //	Opcode Address Mode : A function pointer to the implementation of the
    //						  addressing mechanism used by the instruction
    //	Cycle Count : An integer that represents the base number of clock cycles the
    //				  CPU requires to perform the instruction
    struct INSTRUCTION
    {
        std::string name;
        uint8_t (olc6502::*operate)(void)  = nullptr;
        uint8_t (olc6502::*addrmode)(void) = nullptr;
        uint8_t cycles = 0;
    };
#endif

    // Assisstive variables to facilitate emulation
    Registers _registers;
#if INSTRUCTION_EXECUTOR
    InstructionExecutor _executor;
#else
    uint8_t  _fetched = 0x00; // Represents the working input value to the ALU
    uint16_t _temp = 0x0000; // A convenience variable used everywhere
    uint16_t _addr_abs = 0x0000; // All used memory addresses end up in here
    uint16_t _addr_rel = 0x0000; // Represents absolute address following a branch
    uint8_t  _opcode = 0x00; // Is the instruction byte
    uint8_t  _cycles = 0; // Counts how many cycles the instruction has remaining
    uint32_t _clock_count = 0; // A global accumulation of the number of clocks
    std::vector<INSTRUCTION> _lookup;
#endif
    bool     _log = false;

#if !INSTRUCTION_EXECUTOR
    // Convenience functions to access status register
    uint8_t GetFlag(FLAGS6502 f) const   { return registers().GetFlag(f); }
    void    SetFlag(FLAGS6502 f, bool v) { registers().SetFlag(f, v); }
#endif

    // These only exist to get around the QML type system.  It only really knows about
    // int, which is OK because in this case, all unsigned 8-bit values exist within the
    // positive half of an int.
    int property_a() { return static_cast<int>(a()); }
    int property_x() { return static_cast<int>(x()); }
    int property_y() { return static_cast<int>(y()); }
    int property_stkp() { return static_cast<int>(stackPointer()); }
    int property_pc() { return static_cast<int>(pc()); }
    int property_status() { return static_cast<int>(status()); }
};

#endif // CPU_HPP
