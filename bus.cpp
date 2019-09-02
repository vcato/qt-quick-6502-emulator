#include "bus.hpp"

Bus::Bus(QObject *parent)
    :
    QObject(parent)
{
}

void Bus::write(addressType address, uint8_t data)
{
    emit writeSignal(address, data);
}

uint8_t Bus::read(addressType address, bool read_only)
{
    return emit readSignal(address, read_only);
}
