#ifndef RAMBUSDEVICEDISASSEMBLYMODEL_HPP
#define RAMBUSDEVICEDISASSEMBLYMODEL_HPP

#include "rambusdevice.hpp"
#include "olc6502.hpp"
#include <QObject>
#include <QString>


class RamBusDeviceDisassemblyModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(RamBusDevice *memory             READ memoryModel        WRITE setMemoryModel   NOTIFY memoryModelChanged)
    Q_PROPERTY(olc6502      *cpu                READ cpuModel           WRITE setCpuModel      NOTIFY cpuModelChanged)
    Q_PROPERTY(QString       visibleDisassembly READ visibleDisassembly                        NOTIFY visibleDisassemblyChanged)
    Q_PROPERTY(int           numberOfLines      READ numberOfLines      WRITE setNumberOfLines NOTIFY numberOfLinesChanged)
    Q_PROPERTY(int           startAddress       READ startAddress       WRITE setStartAddress  NOTIFY startAddressChanged)
    Q_PROPERTY(int           endAddress         READ endAddress         WRITE setEndAddress    NOTIFY endAddressChanged)
public:
    explicit RamBusDeviceDisassemblyModel(QObject *parent = nullptr);

    static void RegisterType();

    /** Retrieve the underlying memory model.
     *
     *  @return A pointer to the underlying memory model
     */
    ///@{
    const RamBusDevice *memoryModel() const { return _memory_model; }
          RamBusDevice *memoryModel()       { return _memory_model; }
    ///@}

    /** Sets the underlying memory model.
     *
     *  @param new_model The memory model to use
     */
    void setMemoryModel(RamBusDevice *new_model);

    /** Retrieve the underlying cpu model.
     *
     *  @return A pointer to the underlying cpu model
     */
    ///@{
    const olc6502 *cpuModel() const { return _cpu_model; }
          olc6502 *cpuModel()       { return _cpu_model; }
    ///@}

    /** Sets the underlying cpu model.
     *
     *  @param new_model The cpu model to use
     */
    void setCpuModel(olc6502 *new_cpu_model);

    QString visibleDisassembly() const { return _visible_disassembly; }
    QString visibleDisassembly()       { return _visible_disassembly; }

    void retrieveDisassembly();

    int numberOfLines() const { return _number_of_lines; }
    int numberOfLines()       { return _number_of_lines; }

    void setNumberOfLines(int lines);

    int startAddress() const { return _start_address; }
    int startAddress()       { return _start_address; }

    void setStartAddress(int address);

    int endAddress() const { return _end_address; }
    int endAddress()       { return _end_address; }

    void setEndAddress(int address);
signals:
    /** Emitted when the underlying memory model is set or reset.
     *
     *  @see memoryModel
     *  @see setMemoryModel
     */
    void memoryModelChanged();

    /** Emitted when the underlying cpu model is set or reset.
     *
     *  @see cpuModel
     *  @see setCpuModel
     */
    void cpuModelChanged();

    /** Emitted when the number of lines is set.
     *
     *  @see numberOfLines
     *  @see setNumberOfLines
     */
    void numberOfLinesChanged();

    void startAddressChanged();
    void endAddressChanged();

    void visibleDisassemblyChanged();
public slots:

private slots:
    void onCpuProgramCounterChanged(uint16_t address);

private:
    RamBusDevice             *_memory_model = nullptr;
    olc6502                  *_cpu_model = nullptr;
    olc6502::disassemblyType  _cpu_disassembly;
    QString                   _visible_disassembly;
    int                       _number_of_lines = 0;
    int                       _start_address   = 0;
    int                       _end_address     = 0;

    void calculateVisibleDisassembly();
    void calculateVisibleDisassemblyIfNecessary();
};

#endif // RAMBUSDEVICEDISASSEMBLYMODEL_HPP
