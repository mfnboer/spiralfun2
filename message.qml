import QtQuick
import QtQuick.Controls

Dialog {
    property string msg: ""

    id: msgDialog
    modal: false
    standardButtons: Dialog.Ok
    anchors.centerIn: parent
    onAccepted: destroy()

    Label {
        Layout.maximumWidth: parent.width
        wrapMode: Text.WordWrap
        text: msgDialog.msg
    }
}
