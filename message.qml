import QtQuick
import QtQuick.Controls

Dialog {
    id: msgDialog
    modal: false
    standardButtons: Dialog.Ok
    anchors.centerIn: parent
    onAccepted: destroy()

    Label {
        id: msgLabel
        textFormat: Text.MarkdownText
    }

    function show(msg) {
        msgLabel.text = msg;
        open();
    }
}
