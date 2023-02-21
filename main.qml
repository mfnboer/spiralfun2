import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 480
    height: 960
    visible: true
    title: "Spiral Fun"

    GridLayout {
        columns: 4
        anchors.fill: parent

        // Row 1
        Rectangle {
            color: "black"
            Layout.columnSpan: 4
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        // Row 2
        Button {
            text: "Up"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.preferredWidth: root.width / 2
            Layout.minimumWidth: root.width / 2
            Layout.maximumWidth: root.width / 2
        }
        Button {
            text: "Down"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.preferredWidth: root.width / 2
            Layout.minimumWidth: root.width / 2
            Layout.maximumWidth: root.width / 2
        }

        // Row 3
        Label {
            text: "Diameter:"
        }
        SpinBox {
            from: 1; to: 300
            editable: true
            Layout.fillWidth: true
        }
        Button {
            id: colorButton
            Material.background: Material.Yellow
            Layout.fillWidth: true

            onClicked: {
                var component = Qt.createComponent("color_selector.qml");
                var cs = component.createObject(root);
                cs.currentColor = colorButton.background.color
                cs.onAccepted.connect(() => colorButton.background.color = cs.color)
                cs.open()
            }
        }
        CheckBox {
            text: "draw line"
            Layout.fillWidth: true
        }

        // Row 4
        Label {
            text: "Rotations:"
        }
        SpinBox {
            id: spin
            from: 1; to: 9999
            editable: true
            Layout.fillWidth: true

            textFromValue: function(value, locale) { return value.toString(); }
        }
        ComboBox {
            model: ["clockwise", "counter clockwise"]
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        // Row 5
        Label {
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
