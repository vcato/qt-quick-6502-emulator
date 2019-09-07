#ifndef COMPUTER_HPP
#define COMPUTER_HPP

#include <QObject>
#include <QTimer>
#include "olc6502.hpp"
#include "bus.hpp"
#include "rambusdevice.hpp"


class Computer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(olc6502 *cpu READ cpu CONSTANT FINAL)
public:
    explicit Computer(QObject *parent = nullptr);

    static void RegisterType();

public slots:
    void startClock();
    void stopClock();
    void stepClock();

    olc6502 *cpu() { return &_cpu; }

signals:

private slots:
    void timerTimeout();

private:
    olc6502 _cpu;
    Bus _bus;
    RamBusDevice _memory;
    QTimer       _clock;

    void loadProgram();

    Q_DISABLE_COPY(Computer)
};

#endif // COMPUTER_HPP
