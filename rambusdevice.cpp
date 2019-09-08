#include "rambusdevice.hpp"
#include <QtQml>
#include <algorithm>


RamBusDevice::RamBusDevice()
    :
    IBusDevice(0x0000, 0xFFFF, true, true)
{
    std::fill( std::begin(_data), std::end(_data), 0);
}

RamBusDevice::~RamBusDevice()
{
}

void RamBusDevice::RegisterType()
{
    // We won't be creating any of these in QML, so let's
    // prevent the user from creating any by using this version
    // of qmlRegisterType.
    qmlRegisterType<RamBusDevice>();
}

void RamBusDevice::writeImplementation(uint16_t address, uint8_t data)
{
    _data[address] = data;
    emit memoryChanged(address, data);
}

uint8_t RamBusDevice::readImplementation(uint16_t address, bool read_only)
{
    Q_UNUSED(read_only);

    return _data[address];
}
