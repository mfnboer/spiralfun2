import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    property int imgSize
    property string selected
    property list<string> toDelete
    property bool deleteCheckVisible: false

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
            cellHeight: fileDialog.imgSize + 30
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: deleteButton.top
            clip: true
            delegate: Column {
                Image {
                    id: img
                    source: modelData.imgFileName ? "file://" + modelData.imgFileName : "/images/question_mark.png"
                    width: fileDialog.imgSize
                    height: fileDialog.imgSize
                    anchors.horizontalCenter: parent.horizontalCenter

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (fileDialog.deleteCheckVisible)
                                deleteCheckBox.toggle()
                            else
                                fileDialog.selectFile(modelData.jsonFileName)
                        }
                        onPressAndHold: {
                            fileDialog.deleteCheckVisible = true;
                            deleteCheckBox.checked = true
                        }
                    }

                    CheckBox {
                        id: deleteCheckBox
                        visible: fileDialog.deleteCheckVisible
                        onCheckedChanged: {
                            if (checked)
                                addToDelete(modelData.jsonFileName)
                            else
                                removeFromDelete(modelData.jsonFileName)
                        }
                    }
                }
                Label {
                    id: displayLabel
                    text: modelData.displayName
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Button {
            id: deleteButton
            text: "Delete"
            visible: fileDialog.deleteCheckVisible
            enabled: fileDialog.toDelete.length > 0
            anchors.bottom: parent.bottom
            onClicked: confirmDeleteDialog.open()
        }
    }

    function addToDelete(file) {
        if (!fileDialog.toDelete.includes(file))
            fileDialog.toDelete.push(file);
    }

    function removeFromDelete(file) {
        fileDialog.toDelete = fileDialog.toDelete.filter(item => item !== file);
        if (fileDialog.toDelete.length === 0)
            fileDialog.deleteCheckVisible = false;
    }

    function selectFile(file) {
        fileDialog.selected = file;
        fileDialog.toDelete = [];
        accept();
    }

    function show(fileList, imgSz)
    {
        grid.model = fileList;
        imgSize = imgSz;
        open();
    }

    Dialog {
        id: confirmDeleteDialog
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        Label {
            text: confirmDeleteDialog.question()
        }

        function question() {
            var n = fileDialog.toDelete.length;
            return "Delete " + n + " configuration" + (n > 1 ? "s" : "") + "?";
        }

        onAccepted: fileDialog.accept()
    }
}
