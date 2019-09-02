#ifndef COMPUTER_HPP
#define COMPUTER_HPP

#include <QObject>
#include <QTimer>
#include "cpu.hpp"
#include "bus.hpp"
#include "rambusdevice.hpp"


class Computer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(CPU *cpu READ cpu CONSTANT FINAL)
public:
    explicit Computer(QObject *parent = nullptr);

    static void RegisterType();

public slots:
    void startClock();
    void stopClock();
    void stepClock();

    CPU *cpu() { return &_cpu; }

signals:

private slots:
    void timerTimeout();

private:
    CPU _cpu;
    Bus _bus;
    RamBusDevice _memory;
    QTimer       _clock;

    void loadProgram();

    Q_DISABLE_COPY(Computer)
};

#endif // COMPUTER_HPP
