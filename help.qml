import QtQuick
import QtQuick.Controls

Dialog {
    title: "Help"
    modal: true
    standardButtons: Dialog.Ok
    anchors.centerIn: parent
    onAccepted: destroy()

    Label {
        anchors.fill: parent
        wrapMode: Text.WordWrap
        textFormat: Text.MarkdownText
        text: "
Spiral lines are drawn by circles rotating around each other. \
You can create up to 10 cirles (set in the 'Circles' field).

The first circle is stationary, the second cirle makes 1 rotation. \
For the remaining circles you can set how many rotations to make in \
which direction.

For each circle you can set its diameter, its color and whether it \
should draw (the draw line checkbox) a line from its center while rotating. \

With the up and down arrow buttons you can select the circle to configure.

Press the play button to start the rotations."
    }
}
