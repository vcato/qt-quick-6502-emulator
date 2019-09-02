#ifndef CPU_HPP
#define CPU_HPP

#include <QObject>
#include <QPointer>
#include <string>

class CPU : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int a            READ property_a      NOTIFY aChanged)
    Q_PROPERTY(int x            READ property_x      NOTIFY xChanged)
    Q_PROPERTY(int y            READ property_y      NOTIFY yChanged)
    Q_PROPERTY(int stackPointer READ property_stkp   NOTIFY stackPointerChanged)
    Q_PROPERTY(int pc           READ property_pc     NOTIFY pcChanged)
    Q_PROPERTY(int status       READ property_status NOTIFY statusChanged)
public:
    using addressType = uint16_t;

    enum FLAGS6502
    {
        C = (1 << 0),
        Z = (1 << 1),
        I = (1 << 2),
        D = (1 << 3),
        B = (1 << 4),
        U = (1 << 5),
        V = (1 << 6),
        N = (1 << 7)
    };
    Q_ENUM(FLAGS6502)

    explicit CPU(QObject *parent = nullptr);

    static void RegisterType();

    // Addressing Modes
    uint8_t IMP(); uint8_t IMM();
    uint8_t ZP0(); uint8_t ZPX();
    uint8_t ZPY(); uint8_t REL();
    uint8_t ABS(); uint8_t ABX();
    uint8_t ABY(); uint8_t IND();
    uint8_t IZX(); uint8_t IZY();

    // Opcodes
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

    uint8_t XXX(); // illegal opcode

    void clock();
    void reset();
    void irq();
    void nmi();

    uint8_t fetch();

    // Indicates the current instruction has completed by returning true. This is
    // a utility function to enable "step-by-step" execution, without manually
    // clocking every cycle
    bool complete();

    uint8_t a() const { return _a; }
    uint8_t x() const { return _x; }
    uint8_t y() const { return _y; }

    uint16_t pc() const { return _pc; }
    uint8_t  stackPointer() const { return _stkp; }
    uint8_t  status() const { return _status; }

public slots:
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

private:
    struct INSTRUCTION
    {
        std::string name;
        uint8_t (CPU::*operate)(void)  = nullptr;
        uint8_t (CPU::*addrmode)(void) = nullptr;
        uint8_t cycles = 0;
    };

    uint8_t  _a = 0x00;
    uint8_t  _x = 0x00;
    uint8_t  _y = 0x00;
    uint8_t  _stkp = 0x00;
    uint16_t _pc = 0x0000;
    uint8_t  _status = 0x00; // status register
    uint8_t  _fetched = 0x00;
    uint16_t _temp = 0x0000;
    uint16_t _addr_abs = 0x0000;
    uint16_t _addr_rel = 0x0000;
    uint8_t  _opcode = 0x00;
    uint8_t  _cycles = 0;
    uint32_t _clock_count = 0;
    std::vector<INSTRUCTION> _lookup;

    uint8_t GetFlag(FLAGS6502 f);
    void    SetFlag(FLAGS6502 f, bool v);

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
