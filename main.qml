import QtCore
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

                function donePlaying() {
                    return playState === SpiralScene.DONE_PLAYING;
                }

                function playStateIcon() {
                    switch (scene.playState) {
                    case SpiralScene.NOT_PLAYING:
                        return "play";
                    case SpiralScene.PLAYING:
                        return "stop";
                    case SpiralScene.DONE_PLAYING:
                        return "home";
                    }
                }
            }
        }

        // Row 2
        Button {
            icon.name: "arrow_upward"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.minimumWidth: root.width / 2 - 5
            Layout.maximumWidth: root.width / 2 - 5
            Layout.leftMargin: 5
            enabled: scene.currentCircleIndex < scene.numCircles - 1 && scene.notPlaying()
            onClicked: scene.circleUp()
        }
        Button {
            icon.name: "arrow_downward"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.minimumWidth: root.width / 2 - 5
            Layout.maximumWidth: root.width / 2 - 5
            Layout.rightMargin: 5
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
            Layout.rightMargin: 5
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
            Layout.rightMargin: 5
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
        RowLayout {
            id: buttonRow
            Layout.columnSpan: 2
            Button {
                id: playButton
                icon.name: scene.playStateIcon()
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
                id: saveButton
                icon.name: "save"
                Layout.fillWidth: true
                enabled: scene.donePlaying() && !scene.savedImage
                onClicked: scene.saveImage()
            }
            Button {
                id: shareButton
                icon.name: "share"
                Layout.fillWidth: true
                enabled: scene.donePlaying()
                onClicked: scene.shareImage()
            }
            Button {
                id: moreButton
                icon.name: "more"
                enabled: scene.notPlaying()
                Layout.fillWidth: true
                Layout.rightMargin: 5
                onClicked: { moreMenu.currentIndex = -1; moreMenu.open(); }
            }
        }

        Menu {
            id: moreMenu
            x: parent.width - width
            y: buttonRow.y - height

            MenuItem {
                text: "Examples"
                icon.name: "spiral"
                onTriggered: {
                    var win = moreMenu.showWindow("examples.qml");
                    win.onAccepted.connect(() => scene.setupExample(win.selected));
                }
            }
            MenuItem {
                text: "Help"
                icon.name: "help"
                onTriggered: moreMenu.showWindow("help.qml")
            }
            MenuItem {
                text: "About"
                icon.name: "info"
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
