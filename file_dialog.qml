import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    property list<string> files
    property int imgSize
    property string selected

    id: fileDialog

    title: "Load Spiral Config"
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
            id: list
            interactive: true
            cellWidth: fileDialog.imgSize + 5
            cellHeight: fileDialog.imgSize + 20
            anchors.fill: parent
            clip: true
            delegate: Column {
                Image {
                    id: img
                    source: "file://" + modelData
                    width: fileDialog.imgSize
                    height: fileDialog.imgSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    MouseArea {
                        anchors.fill: parent
                        onClicked: { console.debug(index, modelData); list.currentIndex = index }
                        onDoubleClicked: { fileDialog.selected = modelData; accept() }
                    }
                }
                Label {
                    text: "foo"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

            }

            highlight: Rectangle {
                color: "grey"
            }
            highlightFollowsCurrentItem: true

            model: fileDialog.files
        }
    }

    function show(fileList, imgSz)
    {
        files = fileList;
        imgSize = imgSz;
        open();
    }
}
