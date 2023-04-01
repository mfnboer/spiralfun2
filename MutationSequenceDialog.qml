import Qt.labs.qmlmodels
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SpiralFun

Dialog {
    property int numCircles
    property list<Mutation> mutationList
    property int sequenceLength
    property bool addReverseSequence
    property list<string> circleColorList
    property int saveAs: MutationSequence.SAVE_AS_NONE
    property bool saveInNewAlbum: true
    property int frameRate: GifRecorder.FPS_10

    id: mutationSequenceDialog
    standardButtons: Dialog.Cancel | Dialog.Ok
    height: parent.height
    width: parent.width
    anchors.centerIn: parent

    header: RowLayout {
        Label {
            text: "Mutation sequence"
            font.bold: true
            font.pointSize: 20
            Layout.fillWidth: true
            leftPadding: 25
        }
        RoundButton {
            icon.name: "more"
            Material.background: "transparent"
            onClicked: moreMenu.open()
        }

        Menu {
            id: moreMenu
            x: parent.width - width

            MenuItem {
                text: "Help"
                onTriggered: {
                    var component = Qt.createComponent("Help.qml");
                    var obj = component.createObject(root);
                    obj.openMutationSequenceHelp();
                }
            }
        }
    }

    ListView {
        id: sequenceListView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: addButton.top

        model: mutationList

        delegate: GridLayout {
            columns: 4
            width: parent.width

            ComboBox {
                id: circleComboBox
                model: { var l = []; for (let i = numCircles; i > 0; --i) { l.push(`Circle ${i}`); }; return l; }
                Material.foreground: circleColorList[indexToCircle(currentIndex)]
                Material.background: "black"
                popup.background: Rectangle { color: "black" }
                currentIndex: numCircles - modelData.circle - 1
                Layout.fillWidth: true
                onActivated: {
                    modelData.circle = indexToCircle(currentIndex)

                    if (modelData.circle === 0) {
                        modelData.trait = Mutation.TRAIT_DIAMETER
                    }
                }

                delegate: ItemDelegate {
                    contentItem: Label {
                        text: modelData
                        color: circleColorList[circleComboBox.indexToCircle(index)]
                    }
                }

                function indexToCircle(idx)
                {
                    return numCircles - idx - 1
                }
            }

            ComboBox {
                id: traitsComboBox
                model: modelData.circle > 0 ? ["diameter", "rotations", "direction"] : ["diameter"]
                currentIndex: modelData.trait
                Layout.fillWidth: true
                onActivated: {
                    modelData.trait = currentIndex

                    if (modelData.trait === Mutation.TRAIT_DIRECTION) {
                        modelData.change = Mutation.CHANGE_INCREMENT
                    }
                }
            }

            ComboBox {
                id: changeComboBox
                model: modelData.trait === Mutation.TRAIT_DIRECTION ? ["flip"] : ["+1", "-1"]
                currentIndex: modelData.change
                Layout.preferredWidth: 70
                onActivated: modelData.change = currentIndex
            }

            RoundButton {
                icon.name: "delete"
                Material.background: "transparent"
                onClicked: {
                    var l = []
                    for (let i = 0; i < mutationList.length; ++i) {
                        if (i === index) {
                            mutationList[i].destroy()
                        } else {
                            l.push(mutationList[i])
                        }
                    }
                    mutationList = l
                }
            }
        }
    }

    Button {
        id: addButton
        anchors.bottom: settingsGrid.top
        text: "Add mutation"
        onClicked: addMutation()
    }

    GridLayout {
        id: settingsGrid
        anchors.bottom: parent.bottom
        columns: 2

        Label {
            text: "Sequence length (frames):"
        }
        SpinBox {
            id: sequenceLengthSpinBox
            from: 1; to: 500
            editable: true
            onValueChanged: sequenceLength = value
            Keys.onReturnPressed:  Qt.inputMethod.hide()
        }

        CheckBox {
            id: addReverseSequenceCheckBox
            text: "Add reverse sequence"
            checked: addReverseSequence
            onCheckedChanged: addReverseSequence = checked
            Layout.columnSpan: 2
        }

        Label {
            text: "Save sequence as:"
        }
        ComboBox {
            id: saveAsComboBox
            model: ["none (just play)", "Picture per frame", "GIF"]
            implicitContentWidthPolicy: ComboBox.WidestText
            currentIndex: saveAs
            onActivated: saveAs = currentIndex
        }

        Label {
            text: "GIF frames per second:"
            enabled: saveAs === MutationSequence.SAVE_AS_GIF
        }
        ComboBox {
            id: frameRateComboBox
            model: ["25 (video)", "10 (slow video)", "4 (fast slide show)", "2 (slide show)", "1 (slow slide show)"]
            implicitContentWidthPolicy: ComboBox.WidestText
            currentIndex: frameRate
            enabled: saveAs === MutationSequence.SAVE_AS_GIF
            onActivated: frameRate = currentIndex
        }

        CheckBox {
            id: saveInNewAlbumCheckBox
            text: "Save pictures in new album"
            checked: saveInNewAlbum
            enabled: saveAs === MutationSequence.SAVE_AS_PICS
            onCheckedChanged: saveInNewAlbum = checked
            Layout.columnSpan: 2
        }
    }

    function addMutation() {
        var mutation = MutationFactory.createMutation(numCircles - 1)
        mutationList.push(mutation)
    }

    function show(numCircles, colorList) {
        circleColorList = colorList
        saveAs = MutationSequence.SAVE_AS_NONE

        if (numCircles !== mutationSequenceDialog.numCircles) {
            // If the number of  circles have changed then the current mutations
            // may refer to non-exisiting circles. Cleanup current mutations.
            for (let i = 0; i < mutationList.length; ++i)
                mutationList[i].destroy()

            mutationList = []
            mutationSequenceDialog.numCircles = numCircles
            addMutation()
            sequenceLength = 10
            sequenceLengthSpinBox.value = sequenceLength
            addReverseSequence = false
        }

        open()
    }
}
