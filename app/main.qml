import QtQuick 2.05

import QtQuick.Layouts 1.0
//import QtQuick.Layouts 1.3

import QtQuick.Window 2.0
//import QtQuick.Window 2.3

import QtQuick.Controls 1.2
//import QtQuick.Controls 1.4
import Qt.example.computer 1.0
import Qt.example.rambusdeviceview 1.0
import Qt.example.rambusdevicetablemodel 1.0
import Qt.example.rambusdevicedisassemblymodel 1.0

Window {
    visible: true
    width: registers.width + memory_views.width + 20
    height: 1100
    title: qsTr("Hello World")

    RamBusDeviceTableModel {
        id: zero_page_ram_table_model
        memorymodel: Computer.ram
        page: 0x00
    }

    RamBusDeviceDisassemblyModel {
        id: disassembly_model
        numberOfLines: 10
        memory: Computer.ram
        cpu: Computer.cpu
        startAddress: 0x8000
        endAddress: 0x9000
    }

    RowLayout {
        id: clock_control_row
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Button {
            text: "Start Clock"
            Layout.margins: 10
            onClicked: Computer.startClock()
        }
        Button {
            text: "Stop Clock"
            Layout.margins: 10
            onClicked: Computer.stopClock()
        }
        Button {
            text: "Step"
            Layout.margins: 10
            onClicked: Computer.cpu.clock()
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
            Layout.margins: 10

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
            Layout.margins: 10

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
            Layout.margins: 10

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
            Layout.margins: 10

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
            Layout.margins: 10

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
            Layout.margins: 10

            Label {
                text: "STACK P:"
            }
            Label {
                text: Computer.cpu.stackPointer
            }
        }
        RowLayout {
            id: disassembly_row
            Layout.alignment: Qt.AlignLeft | Qt.AlignRight
            Layout.fillWidth: true
            Layout.margins: 10

            Rectangle {
                id: disassembly_rectangle
                color: "blue"
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 650

                Label {
                    id: disassembly_label
                    text: disassembly_model.visibleDisassembly
                    font.family: "Lucida Console"
                    font.pointSize: 12
                    color: "white"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.Center
                }
            }
        }
    }
    ColumnLayout {
        id: memory_views
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: clock_control_row.top

        TableView {
            id: zero_page_ram_view

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10
            model: zero_page_ram_table_model

            TableViewColumn {
                id: addressColumn
                title: "Address"
                role: "address"
                resizable: true
            }

            TableViewColumn {
                id: memoryColumn
                title: "Memory"
                role: "memory"
                resizable: true
                width: zero_page_ram_view.width - memoryColumn.width
            }
        }

        RamBusDeviceView {
            id: program_page_ram_view

            model: Computer.ram
            page: 0x80
        }
    }
}
