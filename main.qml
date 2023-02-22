import QtQml
import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import SpiralFun

ApplicationWindow {
    id: root
    width: 480
    height: 960
    visible: true
    title: "Spiral Fun"

    GridLayout {
        columns: 4
        anchors.fill: parent

        // Row 1
        Rectangle {
            color: "black"
            Layout.columnSpan: 4
            Layout.fillWidth: true
            Layout.fillHeight: true

            SpiralScene {
                id: scene
                width: parent.width
                height: parent.height

                function notPlaying() {
                    return playState === SpiralScene.NOT_PLAYING;
                }

                function playStateText() {
                    switch (scene.playState) {
                    case SpiralScene.NOT_PLAYING:
                        return "Play";
                    case SpiralScene.PLAYING:
                        return "Stop";
                    case SpiralScene.DONE_PLAYING:
                        return "Reset";
                    }
                }
            }
        }

        // Row 2
        Button {
            text: "Up"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.minimumWidth: root.width / 2
            Layout.maximumWidth: root.width / 2
            enabled: scene.currentCircleIndex < scene.numCircles - 1 && scene.notPlaying()
            onClicked: scene.circleUp()
        }
        Button {
            text: "Down"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.minimumWidth: root.width / 2
            Layout.maximumWidth: root.width / 2
            enabled: scene.currentCircleIndex > 0 && scene.notPlaying()
            onClicked: scene.circleDown()
        }

        // Row 3
        Label {
            text: "Diameter:"
        }
        SpinBox {
            id: diameterSpinBox
            from: 1; to: scene.MAX_DIAMETER
            editable: true
            value: scene.currentCircle.diameter
            Layout.fillWidth: true
            enabled: scene.notPlaying()
            onValueChanged: scene.currentCircle.diameter = value
        }
        Button {
            id: colorButton
            Material.background: scene.currentCircle.color
            Layout.fillWidth: true
            enabled: scene.notPlaying()
            onClicked: {
                var component = Qt.createComponent("color_selector.qml");
                var cs = component.createObject(root);
                cs.currentColor = colorButton.background.color
                cs.onAccepted.connect(() => scene.currentCircle.color = cs.color)
                cs.open()
            }
        }
        CheckBox {
            id: drawCheckBox
            text: "draw line"
            checked: scene.currentCircle.draw
            Layout.fillWidth: true
            enabled: scene.currentCircleIndex > 0 && scene.notPlaying()
            onCheckedChanged: scene.currentCircle.draw = checked
        }

        // Row 4
        Label {
            text: "Rotations:"
        }
        SpinBox {
            id: rotationsSpinBox
            from: 1; to: scene.MAX_ROTATIONS
            editable: true
            value: scene.currentCircle.rotations
            textFromValue: (value, locale) => { return value.toString(); }
            Layout.fillWidth: true
            enabled: scene.currentCircleIndex > 1 && scene.notPlaying()
            onValueChanged: scene.currentCircle.rotations = value
        }
        ComboBox {
            id: directionComboBox
            model: ["clockwise", "counter clockwise"]
            currentIndex: scene.currentCircle.direction
            Layout.columnSpan: 2
            Layout.fillWidth: true
            enabled: scene.currentCircleIndex > 0 && scene.notPlaying()
            onCurrentIndexChanged: scene.currentCircle.direction = currentIndex
        }

        // Row 5
        Label {
            text: "Circles:"
        }
        SpinBox {
            id: circlesSpinBox
            from: scene.MIN_CIRCLES; to: scene.MAX_CIRCLES
            editable: true
            value: scene.numCircles
            Layout.fillWidth: true
            enabled: scene.notPlaying()
            onValueChanged: scene.numCircles = value
        }
        Button {
            id: playButton
            text: scene.playStateText()
            Layout.fillWidth: true
            onClicked: {
                if (scene.playState === SpiralScene.NOT_PLAYING) {
                    scene.play();
                } else {
                    scene.stop();
                }
            }
        }
        Button {
            text: "\u2630"
            enabled: scene.notPlaying()
            Layout.fillWidth: true
        }
    }

    Component.onCompleted: {
        scene.init()
        scene.setupCircles()
    }
}
