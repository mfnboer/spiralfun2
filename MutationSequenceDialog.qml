import Qt.labs.qmlmodels
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SpiralFun

Dialog {
    property int numCircles
    property list<Mutation> mutationList
    property int sequenceLength
    property list<string> circleColorList

    id: mutationSequenceDialog
    title: "Mutation sequence"
    standardButtons: Dialog.Cancel | Dialog.Ok
    height: parent.height
    width: parent.width
    anchors.centerIn: parent

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
                model: { var l = []; for (let i = 0; i < numCircles; ++i) { l.push(`Circle ${i}`); }; return l; }
                Material.foreground: circleColorList[currentIndex]
                Material.background: "black"
                currentIndex: modelData.circle
                Layout.fillWidth: true
                onActivated: {
                    modelData.circle = currentIndex

                    if (modelData.circle === 0) {
                        modelData.trait = Mutation.TRAIT_DIAMETER
                    }
                }

                delegate: ItemDelegate {
                    contentItem: Label {
                        text: modelData
                        color: circleColorList[index]
                    }
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
        anchors.bottom: sequenceLengthRow.top
        text: "Add mutation"
        onClicked: addMutation()
    }

    RowLayout {
        id: sequenceLengthRow
        anchors.bottom: parent.bottom
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
    }

    function addMutation() {
        var mutation = MutationFactory.createMutation(numCircles - 1)
        mutationList.push(mutation)
    }

    function show(numCircles, colorList) {
        circleColorList = colorList

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
        }

        open()
    }
}
