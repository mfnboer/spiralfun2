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

        delegate: RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right

            ComboBox {
                model: { var l = []; for (let i = 0; i < numCircles; ++i) { l.push(`Circle ${i}`); }; return l; }
                Material.foreground: circleColorList[currentIndex]
                Material.background: "black"
                currentIndex: modelData.circle
                onActivated: modelData.circle = currentIndex
                onCurrentIndexChanged: {
                    if (currentIndex === 0) {
                        modelData.trait = Mutation.TRAIT_DIAMETER
                        modelData.change = Mutation.CHANGE_INCREMENT
                    }
                }

                Layout.fillWidth: true

                delegate: ItemDelegate {
                    contentItem: Label {
                        text: modelData
                        color: circleColorList[index]
                    }
                }
            }

            ComboBox {
                model: modelData.circle > 0 ? ["diameter", "rotations", "direction"] : ["diameter"]
                currentIndex: modelData.trait
                onActivated: modelData.trait = currentIndex
                Layout.fillWidth: true
            }

            ComboBox {
                model: modelData.trait === Mutation.TRAIT_DIRECTION ? ["invert"] : ["increment", "decrement"]
                currentIndex: modelData.change
                onActivated: modelData.change = currentIndex
                Layout.fillWidth: true
            }

            Button {
                icon.name: "delete"
                onClicked: {
                    var l = []
                    for (let i = 0; i < mutationList.length; ++i) {
                        if (i !== index) {
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
        mutationList.push(Qt.createQmlObject(`
            import SpiralFun
            Mutation { circle: ${(numCircles - 1)} }`,
            mutationSequenceDialog));
    }

    function show(numCircles, colorList) {
        circleColorList = colorList

        if (numCircles !== mutationSequenceDialog.numCircles) {
            // If the number of  circles have changed then the current mutations
            // may refer to non-exisiting circles. Cleanup current mutations.
            mutationSequenceDialog.numCircles = numCircles
            mutationList = []
            addMutation()
            sequenceLength = 10
            sequenceLengthSpinBox.value = sequenceLength
        }

        open()
    }
}
