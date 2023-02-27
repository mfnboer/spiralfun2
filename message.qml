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
        wrapMode: Text.WordWrap
        text: msgDialog.msg
    }
}
