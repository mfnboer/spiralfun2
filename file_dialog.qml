import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    property int imgSize
    property string selected

    id: fileDialog
    title: "Load spiral config"
    modal: true
    standardButtons: Dialog.Cancel
    height: parent.height
    width: parent.width
    anchors.centerIn: parent
    onAccepted: destroy()
    onRejected: destroy()

    Rectangle {
        color: "transparent"
        anchors.fill: parent

        GridView {
            id: grid
            cellWidth: fileDialog.imgSize + 5
            cellHeight: fileDialog.imgSize + 20
            anchors.fill: parent
            clip: true
            delegate: Column {
                Image {
                    source: modelData.imgFileName ? "file://" + modelData.imgFileName : "/images/question_mark.png"
                    width: fileDialog.imgSize
                    height: fileDialog.imgSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    MouseArea {
                        anchors.fill: parent
                        onClicked: { fileDialog.selected = modelData.jsonFileName; accept() }
                    }
                }
                Label {
                    text: modelData.displayName
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    function show(fileList, imgSz)
    {
        grid.model = fileList;
        imgSize = imgSz;
        open();
    }
}
