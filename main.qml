import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import Qt.example.computer 1.0
import Qt.example.rambusdeviceview 1.0

Window {
    visible: true
    width: 800
    height: 600
    title: qsTr("Hello World")

    RowLayout {
        id: clock_control_row
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Button {
            text: "Start Clock"
            onClicked: Computer.startClock()
        }
        Button {
            text: "Stop Clock"
            onClicked: Computer.stopClock()
        }
    }
    ColumnLayout {
        id: registers
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: clock_control_row.top

        RowLayout {
            id: status_row
            spacing: 10
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true

            Label {
                text: "STATUS:"
            }
            Label {
                text: Computer.cpu.status
            }
        }
        RowLayout {
            id: pc_row
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true

            Label {
                text: "PC:"
            }
            Label {
                text: Computer.cpu.pc
            }
        }
        RowLayout {
            id: accumulator_row
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true

            Label {
                text: "A:"
            }
            Label {
                text: Computer.cpu.a
            }
        }
        RowLayout {
            id: x_register_row
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true

            Label {
                text: "X:"
            }
            Label {
                text: Computer.cpu.x
            }
        }
        RowLayout {
            id: y_register_row
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true

            Label {
                text: "Y:"
            }
            Label {
                text: Computer.cpu.y
            }
        }
        RowLayout {
            id: stack_pointer_row
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true

            Label {
                text: "STACK P:"
            }
            Label {
                text: Computer.cpu.stackPointer
            }
        }
    }
    ColumnLayout {
        id: memory_views
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: clock_control_row.top

        RamBusDeviceView {
            id: zero_page_ram_view

            model: Computer.ram
            page: 0x00
        }
        RamBusDeviceView {
            id: program_page_ram_view

            model: Computer.ram
            page: 0x80
        }
    }
}
