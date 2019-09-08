#include "rambusdeviceview.hpp"
#include <QPainter>
#include <QtQml>
#include <QTextStream>
#include <QDebug>

namespace
{
const int DisplayCellsOfByte = 3;
const int DisplayCellsOfAddress = 4;
const char AddressPrefixChar = '$';
const char AddressPostfixChar = ':';
const int LineNumberOfCells = DisplayCellsOfAddress + 2 + 16 * DisplayCellsOfByte;

uint16_t addressOfLine(int page, int linenumber)
{
    return static_cast<uint16_t>((page << 8) + (16 * linenumber));
}

QString dataAsHexChars(const RamBusDevice::memory_type &memory, uint16_t address)
{
    QByteArray x(reinterpret_cast<const char *>(memory.data() + address), 16);

    return x.toHex(' ');
}

QString generateLineOfText(const RamBusDevice::memory_type &memory, uint16_t address)
{
    QString line_address = QString("$%1: ").arg(address, 4, 16, QLatin1Char('0'));

    return line_address.append( dataAsHexChars(memory, address) );
}

QString generatePageOfText(const RamBusDevice::memory_type &memory, uint16_t address, int page)
{
    QString text;

    for (int linenumber = 0; linenumber < 16; ++linenumber)
    {
        if (linenumber > 0)
            text.append("\n");
        text.append( generateLineOfText(memory, addressOfLine(page, linenumber)) );
    }
    return text;
}
}

RamBusDeviceView::RamBusDeviceView(QQuickItem *parent)
    :
    QQuickPaintedItem(parent)
{
    QFontMetrics metrics(_font);
    QSize        window_size(LineNumberOfCells * metrics.averageCharWidth(), 16 * metrics.height());

    setImplicitWidth(window_size.width());
    setImplicitHeight(window_size.height());
    setFillColor(Qt::GlobalColor::blue);
}

void RamBusDeviceView::RegisterType()
{
    qmlRegisterType<RamBusDeviceView>("Qt.example.rambusdeviceview",
                                       1,
                                       0,
                                       "RamBusDeviceView");
}

void RamBusDeviceView::setModel(RamBusDevice *new_model)
{
    if (new_model != _model)
    {
        _model->disconnect(_model, &RamBusDevice::memoryChanged,
                           this,   &RamBusDeviceView::onMemoryChanged);
        _model = new_model;
        new_model->connect(new_model, &RamBusDevice::memoryChanged,
                           this,      &RamBusDeviceView::onMemoryChanged);
        emit modelChanged();
    }
}

void RamBusDeviceView::setPage(int new_page)
{
    if (new_page != _page)
    {
        _page = new_page;
        emit pageChanged();
    }
}

void RamBusDeviceView::onMemoryChanged(RamBusDevice::addressType address, uint8_t value)
{
    Q_UNUSED(address);
    Q_UNUSED(value);

    QQuickPaintedItem::update();
}

void RamBusDeviceView::paint(QPainter *painter)
{
    if (!model())
        return;
    painter->setPen(_pen);
    painter->setFont(_font);
    QString content = generatePageOfText(model()->memory(), addressOfLine(page(), 0), page());

    painter->drawText(boundingRect(), content);
}
