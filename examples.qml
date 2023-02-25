import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    property string selected

    id: examplesDialog
    title: "Examples"
    modal: true
    standardButtons: Dialog.Cancel
    anchors.centerIn: parent
    onAccepted: destroy()
    onRejected: destroy()

    ColumnLayout {
        Grid {
            columns: 2

            Button {
                display: AbstractButton.IconOnly
                icon.source: "/images/example1.png"
                icon { width: 150; height: 150; color: "transparent" }
                onClicked: { examplesDialog.selected = icon.source; accept(); }
            }
            Button {
                display: AbstractButton.IconOnly
                icon.source: "/images/example2.png"
                icon { width: 150; height: 150; color: "transparent" }
                onClicked: { examplesDialog.selected = icon.source; accept(); }
            }
            Button {
                display: AbstractButton.IconOnly
                icon.source: "/images/example3.png"
                icon { width: 150; height: 150; color: "transparent" }
                onClicked: { examplesDialog.selected = icon.source; accept(); }
            }
            Button {
                display: AbstractButton.IconOnly
                icon.source: "/images/example4.png"
                icon { width: 150; height: 150; color: "transparent" }
                onClicked: { examplesDialog.selected = icon.source; accept(); }
            }
        }

        Label {
            text: "Tap an example to play."
        }
    }
}
