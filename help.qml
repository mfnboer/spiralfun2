import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    title: "Help"
    modal: true
    standardButtons: Dialog.Ok
    anchors.centerIn: parent
    onAccepted: destroy()

    ColumnLayout {
        Label {
            Layout.maximumWidth: parent.width
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            text: "Spiral lines are drawn by circles rotating around each other. " +
                  "You can create up to 10 circles (number in the 'Circles' field)." +
                  "<p>" +
                  "The first circle is stationary, the second cirle makes 1 rotation. " +
                  "For the remaining circles you can set how many rotations to make in" +
                  "which direction." +
                  "<p>" +
                  "For each circle you can set its diameter, its color and whether it " +
                  "should draw (the draw line checkbox) a line from its center while rotating." +
                  "<p>" +
                  "To select a circle you can tap it or navigate to it with the arrow buttons."
        }

        Grid {
            columns: 2
            spacing: 5
            verticalItemAlignment: Grid.AlignVCenter

            Button {
                icon.name: "arrow_upward"
            }
            Label {
                text: "Go to the circle above the current."
            }
            Button {
                icon.name: "arrow_downward"
            }
            Label {
                text: "Go to the circle below the current."
            }
            Button {
                icon.name: "play"
            }
            Label {
                text: "Let the circles rotate and draw."
            }
            Button {
                icon.name: "stop"
            }
            Label {
                text: "Stop playing."
            }
            Button {
                icon.name: "home"
            }
            Label {
                text: "Clear the drawing. Restore the circles."
            }
        }
    }
}
