#include "rambusdevicedisassemblymodel.hpp"
#include <QtQml>


RamBusDeviceDisassemblyModel::RamBusDeviceDisassemblyModel(QObject *parent)
    :
    QObject(parent)
{
}

void RamBusDeviceDisassemblyModel::RegisterType()
{
    qmlRegisterType<RamBusDeviceDisassemblyModel>("Qt.example.rambusdevicedisassemblymodel",
                                            1,
                                            0,
                                            "RamBusDeviceDisassemblyModel");
}

void RamBusDeviceDisassemblyModel::setMemoryModel(RamBusDevice *new_model)
{
    if (new_model != _memory_model)
    {
        if (_memory_model)
        {
//            _memory_model->disconnect(_memory_model, &RamBusDevice::memoryChanged,
//                                      this,          &RamBusDeviceDisassemblyModel::onMemoryChanged);
        }
        _memory_model = new_model;

        if (new_model)
        {
//               new_model->connect(new_model, &RamBusDevice::memoryChanged,
//                                  this,      &RamBusDeviceDisassemblyModel::onMemoryChanged);

        }
        emit memoryModelChanged();

        calculateVisibleDisassemblyIfNecessary();
    }
}

void RamBusDeviceDisassemblyModel::setCpuModel(olc6502 *new_cpu_model)
{
    if (new_cpu_model != _cpu_model)
    {
        // Don't forget to disconnect the old model...
        if (_cpu_model)
        {
            new_cpu_model->disconnect(new_cpu_model, &olc6502::pcChanged,
                                      this,          &RamBusDeviceDisassemblyModel::onCpuProgramCounterChanged);
        }
        _cpu_model = new_cpu_model;

        // and wire up the new one...
        if (new_cpu_model)
        {
            new_cpu_model->connect(new_cpu_model, &olc6502::pcChanged,
                                   this,          &RamBusDeviceDisassemblyModel::onCpuProgramCounterChanged);
        }
        emit cpuModelChanged();

        calculateVisibleDisassemblyIfNecessary();
    }
}

void RamBusDeviceDisassemblyModel::setNumberOfLines(int lines)
{
    if (lines != _number_of_lines)
    {
        _number_of_lines = lines;
        emit numberOfLinesChanged();
    }
}

void RamBusDeviceDisassemblyModel::setStartAddress(int address)
{
    if (address != _start_address)
    {
        _start_address = address;
        emit startAddressChanged();

        calculateVisibleDisassemblyIfNecessary();
    }
}

void RamBusDeviceDisassemblyModel::setEndAddress(int address)
{
    if (address != _end_address)
    {
        _end_address = address;
        emit endAddressChanged();

        calculateVisibleDisassemblyIfNecessary();
    }
}

void RamBusDeviceDisassemblyModel::calculateVisibleDisassemblyIfNecessary()
{
    if ((startAddress() < endAddress()) && memoryModel() && cpuModel())
    {
        retrieveDisassembly();
        calculateVisibleDisassembly();
    }
}

void RamBusDeviceDisassemblyModel::onCpuProgramCounterChanged(uint16_t address)
{
    Q_UNUSED(address)

    calculateVisibleDisassembly();
}

void RamBusDeviceDisassemblyModel::retrieveDisassembly()
{
    if (memoryModel() && cpuModel())
        _cpu_disassembly = cpuModel()->disassemble( memoryModel()->lowerAddress(),
                                                    memoryModel()->upperAddress());
    else
        _cpu_disassembly.clear();
}

void RamBusDeviceDisassemblyModel::calculateVisibleDisassembly()
{
    QString newDisassembly;
    QString newLine          = "<br>";
    QString colorStart       = "<font color='cyan'>";
    QString colorEnd         = "</font>";
    int     currentLineRange = 0;
    int     linesHalfRange   = numberOfLines() / 2;

    // First, construct the second half...
    for (auto currentLine = _cpu_disassembly.find( cpuModel()->pc());
         (currentLine != std::end(_cpu_disassembly)) && (currentLineRange < linesHalfRange);
         ++currentLine, ++currentLineRange)
    {
        if (newDisassembly.size() > 0)
            newDisassembly.append(newLine);
        if (currentLine->first == cpuModel()->pc())
            newDisassembly.append(colorStart);
        newDisassembly.append( QString(currentLine->second.c_str()) );
        if (currentLine->first == cpuModel()->pc())
            newDisassembly.append(colorEnd);
    }

    // And now construct the first half...
    currentLineRange = 0;
    for (auto currentLine = _cpu_disassembly.find( cpuModel()->pc());
         (currentLine != std::end(_cpu_disassembly)) && (currentLineRange < linesHalfRange);
         --currentLine, ++currentLineRange)
    {
        newDisassembly.prepend( QString(currentLine->second.c_str()).append(newLine) );
    }

    if (newDisassembly != visibleDisassembly())
    {
        _visible_disassembly = newDisassembly;
        emit visibleDisassemblyChanged();
    }
}
