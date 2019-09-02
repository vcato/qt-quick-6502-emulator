#ifndef RAMBUSDEVICE_HPP
#define RAMBUSDEVICE_HPP

#include "ibusdevice.hpp"
#include <array>


class RamBusDevice : public IBusDevice
{
public:
    RamBusDevice();
   ~RamBusDevice() override;

public slots:

protected:
    void    writeImplementation(addressType address, uint8_t data) override;
    uint8_t readImplementation(addressType address, bool read_only) override;

private:
    std::array<uint8_t, 64 * 1024> _data; // 64K
};

#endif // RAMBUSDEVICE_HPP
