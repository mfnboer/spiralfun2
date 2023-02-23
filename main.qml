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
            Layout.leftMargin: 5
        }
        SpinBox {
            id: diameterSpinBox
            from: 1; to: scene.MAX_DIAMETER
            editable: true
            value: scene.currentCircle.diameter
            Layout.fillWidth: true
            enabled: scene.notPlaying()
            onValueModified: scene.currentCircle.diameter = value
            Keys.onReturnPressed:  Qt.inputMethod.hide()
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
            Layout.leftMargin: 5
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
            Keys.onReturnPressed:  Qt.inputMethod.hide()
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
            Layout.leftMargin: 5
        }
        SpinBox {
            id: circlesSpinBox
            from: scene.MIN_CIRCLES; to: scene.MAX_CIRCLES
            editable: true
            value: scene.numCircles
            Layout.fillWidth: true
            enabled: scene.notPlaying()
            onValueChanged: scene.numCircles = value
            Keys.onReturnPressed:  Qt.inputMethod.hide()
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
            id: moreButton
            text: "\u2630"
            enabled: scene.notPlaying()
            Layout.fillWidth: true
            onClicked: moreMenu.open()
        }

        Menu {
            id: moreMenu
            x: parent.width - width
            y: moreButton.y - height

            MenuItem {
                text: "Examples"
                onTriggered: {
                    var win = moreMenu.showWindow("examples.qml");
                    win.onAccepted.connect(() => scene.setupExample(win.selected));
                }
            }
            MenuItem {
                text: "Help"
                onTriggered: moreMenu.showWindow("help.qml")
            }
            MenuItem {
                text: "About"
                onTriggered: moreMenu.showWindow("about.qml")
            }

            function showWindow(qmlFile) {
                var component = Qt.createComponent(qmlFile);
                var obj = component.createObject(root);
                obj.open()
                return obj
            }
        }
    }

    Component.onCompleted: {
        scene.init()
        scene.setupCircles()
    }
}
