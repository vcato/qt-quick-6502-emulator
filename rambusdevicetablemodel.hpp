#ifndef RAMBUSDEVICETABLEMODEL_HPP
#define RAMBUSDEVICETABLEMODEL_HPP

#include <QAbstractTableModel>
#include <QString>
#include "rambusdevice.hpp"
#include <tuple>


class RamBusDeviceTableModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(RamBusDevice *memorymodel READ memoryModel WRITE setMemoryModel NOTIFY memoryModelChanged)
    Q_PROPERTY(int           page        READ page        WRITE setPage        NOTIFY pageChanged)
public:
    RamBusDeviceTableModel(QObject *parent = nullptr);

    enum Roles {
        AddressRole = Qt::UserRole + 1,
        MemoryRole
    };
    Q_ENUM(Roles)

    QHash<int, QByteArray> roleNames() const override {
        return {
            { AddressRole, "address" },
            { MemoryRole,  "memory" }
        };
    }

    static void RegisterType();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /** Retrieve the underlying model.
     *
     *  @return A pointer to the underlying model
     */
    ///@{
    const RamBusDevice *memoryModel() const { return _model; }
          RamBusDevice *memoryModel()       { return _model; }
    ///@}

    /** Sets the underlying model of this view.
     *
     *  @param new_model The model to view
     */
    void setMemoryModel(RamBusDevice *new_model);

    /** Queries the current page to view.
     *
     *  @return The current page to view
     */
    int  page() const { return _page; }

    /** Sets which page number to view.
     *
     *  @param new_page The new page number to view
     */
    void setPage(int new_page);

signals:
    /** Emitted when the underlying model is set or reset.
     *
     *  @see model
     *  @see setModel
     */
    void memoryModelChanged();

    /** Emitted when the current page is changed.
     *
     *  @see page
     *  @see setPage
     */
    void pageChanged();

private:
    RamBusDevice *_model = nullptr;
    int           _page  = 0x00;

    static constexpr int cells_wide = 17; // 1 address + 16 data values
    static constexpr int lines_high = 16;

private slots:
    /** Catched the memoryChanged signal from @c RamBusDevice
     *
     *  @param address The address that was changed
     *  @param value   The value it was changed to
     */
    void onMemoryChanged(RamBusDevice::addressType address, uint8_t value);

private:
    /** Converts a view's row number to an address within the underlying model.
     *
     *  @param index The row of the view to calculate the address of
     *  @return The memory address of the given row
     */
    uint16_t rowToAddress(int index) const;

#if 0
    uint8_t  indexToData(const QModelIndex &index) const;
#endif

    /** Converts a QModelIndex into an address.
     *
     *  @param index The index from the model
     *  @return The address of the beginning of the row
     */
    uint16_t indexToAddress(const QModelIndex &index) const;

    /** Converts an address into components used for retrieving data to display.
     *
     *  @param address
     *  @return A tuple, where tuple<0> is the address to display
     *          in the view, and tuple<1> is the byte number
     *          within the page.
     *
     *  @pre isDisplayableAddress(address) == true
     */
    std::tuple<int, int> addressToIndex(uint16_t address) const;

    /** Generates the text to display for the Address role.
     *
     *  @param address The memory address
     *  @return The hexadecimal representation of the address
     */
    QString  generateLineHeader(uint16_t address) const;

    /** Generates the text used to display the Memory role.
     *
     *  @param address The memory address
     *  @return The hexadecimal representation of each byte
     */
    QString  generateMemoryLine(uint16_t address) const;

    void     fill();
    bool     isDisplayableAddress(uint16_t address) const;
};

#endif // RAMBUSDEVICETABLEMODEL_HPP
