import QtQuick
import QtQuick.Controls
import SpiralFun

Dialog {
    title: "About"
    modal: true
    standardButtons: Dialog.Ok
    anchors.centerIn: parent
    onAccepted: destroy()

    Label {
        wrapMode: Text.WordWrap
        text: "<b>Spiral Fun</b>" +
              "<p>" +
              "Version: " + scene.VERSION +
              "<p>" +
              "\u00A9 2023 Michel de Boer"
    }
}
