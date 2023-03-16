import QtCore
import QtQml
import QtQuick
import QtQuick.Controls.Material
import QtQuick.Dialogs
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
        rowSpacing: 0

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
                onMessage: (msg) => showMessage(msg)

                function notPlaying() {
                    return playState === SpiralScene.NOT_PLAYING;
                }

                function donePlaying() {
                    return playState === SpiralScene.DONE_PLAYING;
                }

                function isRecording() {
                    return playState === SpiralScene.RECORDING;
                }

                function playStateIcon() {
                    switch (scene.playState) {
                    case SpiralScene.NOT_PLAYING:
                        return "play";
                    case SpiralScene.PLAYING:
                    case SpiralScene.RECORDING:
                        return "stop";
                    case SpiralScene.DONE_PLAYING:
                        return "home";
                    }
                }

                function isInVideoShareMode() {
                    return shareMode === SpiralScene.SHARE_VID;
                }

                function shareButtonText() {
                    switch (scene.shareMode) {
                    case SpiralScene.SHARE_PIC:
                        return "share picture";
                    case SpiralScene.SHARE_VID:
                        return "share gif";
                    }
                }
            }

            RoundButton {
                id: sceneMoreButton
                icon.name: "more"
                Material.background: "transparent"
                anchors.top: parent.top
                anchors.right: parent.right
                onClicked: { sceneMoreMenu.currentIndex = -1; sceneMoreMenu.open(); }
            }

            RoundButton {
                id: sceneShareButton
                icon.name: "share"
                text: scene.shareButtonText()
                Material.background: "transparent"
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                enabled: scene.donePlaying()
                visible: scene.donePlaying()
                onClicked: scene.share()
            }

            Label {
                text: "RECORDING"
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: scene.isRecording()
                SequentialAnimation on color {
                    loops: Animation.Infinite
                    ColorAnimation { from: "white"; to: "black"; duration: 1000 }
                    ColorAnimation { from: "black"; to: "white"; duration: 1000 }
                }
            }

            Menu {
                id: sceneMoreMenu
                x: parent.width - width

                MenuItem {
                    text: "Save image"
                    enabled: scene.donePlaying() && !scene.isInVideoShareMode()
                    onTriggered: scene.saveImage()
                }
                MenuItem {
                    text: "Load config"
                    enabled: scene.notPlaying()
                    onTriggered: {
                        var component = Qt.createComponent("file_dialog.qml");
                        var obj = component.createObject(root);
                        obj.onAccepted.connect(() => sceneMoreMenu.handleFileDialogAccepted(obj));
                        obj.show(scene.getConfigFileList(), scene.CFG_IMAGE_SIZE);
                    }
                }
                MenuItem {
                    text: "Save config"
                    enabled: scene.donePlaying() && !scene.isInVideoShareMode()
                    onTriggered: scene.saveConfig()
                }
                MenuItem {
                    text: "Record GIF"
                    enabled: scene.donePlaying() && !scene.isInVideoShareMode()
                    onTriggered: scene.record()
                }
                MenuItem {
                    text: "Examples"
                    enabled: scene.notPlaying()
                    onTriggered: {
                        var win = sceneMoreMenu.showWindow("examples.qml");
                        win.onAccepted.connect(() => scene.setupExample(win.selected));
                    }
                }
                MenuItem {
                    text: "Help"
                    onTriggered: sceneMoreMenu.showWindow("help.qml")
                }
                MenuItem {
                    text: "About"
                    onTriggered: sceneMoreMenu.showWindow("about.qml")
                }

                function handleFileDialogAccepted(obj) {
                    if (obj.selected)
                        scene.loadConfig(obj.selected);
                    else
                        scene.deleteConfig(obj.toDelete);
                }

                function showWindow(qmlFile) {
                    var component = Qt.createComponent(qmlFile);
                    var obj = component.createObject(root);
                    obj.open();
                    return obj;
                }
            }

            BusyIndicator {
                running: scene.sharingInProgress
                anchors.centerIn: parent
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
                cs.currentColor = colorButton.background.color;
                cs.onAccepted.connect(() => scene.currentCircle.color = cs.color);
                cs.open();
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
            from: 0; to: scene.MAX_ROTATIONS
            editable: true
            value: scene.currentCircle.rotations
            textFromValue: (value, locale) => { return value.toString(); }
            Layout.fillWidth: true
            enabled: scene.currentCircleIndex > 0 && scene.notPlaying()
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
        Button {
            id: playButton
            icon.name: scene.playStateIcon()
            Layout.fillWidth: true
            Layout.columnSpan: 2
            onClicked: {
                if (scene.playState === SpiralScene.NOT_PLAYING) {
                    scene.play();
                } else {
                    scene.stop();
                }
            }
        }
    }

    function showMessage(message) {
        var component = Qt.createComponent("message.qml");
        var obj = component.createObject(root);
        obj.show(message);
    }

    Component.onCompleted: {
        scene.init();
    }
}
