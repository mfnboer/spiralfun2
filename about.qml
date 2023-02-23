import QtQuick
import QtQuick.Controls

Dialog {
    id: about
    title: "About"
    modal: true
    standardButtons: Dialog.Ok
    anchors.centerIn: parent
    onAccepted: destroy()

    Text {
        wrapMode: Text.WordWrap
        text: "
            <b>Spiral Fun</b>
            <p>
            \u00A9 2023 Michel de Boer"
    }
}
