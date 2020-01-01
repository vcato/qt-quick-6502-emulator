#include "rambusdevicetablemodel.hpp"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QtQml>
#include <QtDebug>

namespace
{
QString separate(const QString &s,char c)
{
  QString result = "";
  int n = s.length();

  for (int i=0; i!=n; i+=2) {
    if (i!=0) {
      result += c;
    }

    result += s.mid(i,2);
  }

  return result;
}

QString dataAsHexChars(const RamBusDevice::memory_type &memory, uint16_t address)
{
    QByteArray x(reinterpret_cast<const char *>(memory.data() + address), 16);

    return separate(x.toHex(),' ');
}
}

RamBusDeviceTableModel::RamBusDeviceTableModel(QObject *parent)
    :
    QAbstractTableModel(parent)
{
}

void RamBusDeviceTableModel::RegisterType()
{
    qmlRegisterType<RamBusDeviceTableModel>("Qt.example.rambusdevicetablemodel",
                                            1,
                                            0,
                                            "RamBusDeviceTableModel");
}

int RamBusDeviceTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return lines_high;
}

int RamBusDeviceTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return cells_wide;
}

QVariant RamBusDeviceTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    qDebug() << "index: " << index << "\trole: " << role << endl;
    if (role == AddressRole)
    {
        return QVariant( generateLineHeader( rowToAddress(index.row()) ) );
    }
    else if (role == MemoryRole)
    {
        return QVariant( generateMemoryLine( indexToAddress(index) ) );
    }

    return QVariant();
}

Qt::ItemFlags RamBusDeviceTableModel::flags(const QModelIndex &index) const
{
    // First, prune invalid indices
    if (!index.isValid() || (index.row() >= lines_high) || (index.column() >= cells_wide))
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

void RamBusDeviceTableModel::setPage(int new_page)
{
    if (new_page != page())
    {
        _page = new_page;
        emit pageChanged();
    }
}

void RamBusDeviceTableModel::setMemoryModel(RamBusDevice *new_model)
{
    if (new_model != _model)
    {
        // Disconnect old model if we had one
        if (_model)
        {
            _model->disconnect(_model, &RamBusDevice::memoryChanged,
                               this,   &RamBusDeviceTableModel::onMemoryChanged);
        }
        _model = new_model;

        // Connect new model... if we have one
        if (new_model)
        {
            new_model->connect(new_model, &RamBusDevice::memoryChanged,
                               this,      &RamBusDeviceTableModel::onMemoryChanged);
        }
        fill();
        emit memoryModelChanged();
    }
}

void RamBusDeviceTableModel::onMemoryChanged(RamBusDevice::addressType address, uint8_t value)
{
    Q_UNUSED(address)
    Q_UNUSED(value)

    if (memoryModel() && isDisplayableAddress(address))
    {
        auto        address_index = addressToIndex(address);
        QModelIndex model_index   = index(std::get<0>(address_index), std::get<1>(address_index));

        // Change ONLY the address that changed
        emit dataChanged(model_index, model_index, { MemoryRole });
    }
}

uint16_t RamBusDeviceTableModel::rowToAddress(int row) const
{
    return static_cast<uint16_t>((page() << 8) + (row * 16));
}

#if 0
uint8_t RamBusDeviceTableModel::indexToData(const QModelIndex &index) const
{
    uint16_t address = indexToAddress(index);

    return memoryModel()->memory()[address];
}
#endif
uint16_t RamBusDeviceTableModel::indexToAddress(const QModelIndex &index) const
{
    return rowToAddress(index.row()) + static_cast<uint16_t>(index.column());
}

QString RamBusDeviceTableModel::generateLineHeader(uint16_t address) const
{
    return (memoryModel()) ? QString::asprintf("$%.4X:", address) :
                             QString("$XXXX:");
}

QString RamBusDeviceTableModel::generateMemoryLine(uint16_t address) const
{
    return (memoryModel()) ? dataAsHexChars(memoryModel()->memory(), address) : QString();
}

void RamBusDeviceTableModel::fill()
{
    // Check for nonexistent model.  We can't fill it!
    if (!memoryModel())
        return;
    emit dataChanged(index(0, 0), index(lines_high, cells_wide)); // Include headers
}

bool RamBusDeviceTableModel::isDisplayableAddress(uint16_t address) const
{
    uint16_t lower_limit = static_cast<uint16_t>(page() << 8);
    uint16_t upper_limit = lower_limit + 255;

    return (address >= lower_limit) && (address <= upper_limit);
}

std::tuple<int, int> RamBusDeviceTableModel::addressToIndex(uint16_t address) const
{
    Q_ASSERT(isDisplayableAddress(address));

    return std::make_tuple( (address >> 8) - (static_cast<uint16_t>(page()) ), address & 0x00FF);
}
