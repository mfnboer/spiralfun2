import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    width: 640
    height: 480

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            color: "black"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        GridLayout {
            columns: 4
            rows: 4
            Layout.leftMargin: 5
            Layout.rightMargin: 5

            Button {
                text: "Up"
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }
            Button {
                text: "Down"
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Text {
                text: "Diameter:"
            }
            SpinBox {
                from: 1; to: 300
                editable: true
                Layout.fillWidth: true
            }
            Button {
                text: "Color"
                Layout.fillWidth: true
            }
            Switch {
                text: "draw line"
                Layout.fillWidth: true
            }

            Text {
                text: "Rotations:"
            }
            SpinBox {
                from: 1; to: 9999
                editable: true
                Layout.fillWidth: true
            }
            ComboBox {
                model: ["clockwise", "counter clockwise"]
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Text {
                text: "Circles:"
            }
            SpinBox {
                from: 2; to: 10
                editable: true
                Layout.fillWidth: true
            }
            Button {
                text: "Play"
                Layout.fillWidth: true
            }
            Button {
                text: "\u2630"
                Layout.fillWidth: true
            }
        }
    }
}
