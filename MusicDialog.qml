import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    property bool musicOn: false
    property int speed: 45
    property int minSpeed: speed
    property int maxSpeed: speed
    property int toneDistance: 25
    property int minToneDistance: toneDistance
    property int maxToneDistance: toneDistance

    id: musicDialog
    standardButtons: Dialog.Cancel | Dialog.Ok
    height: parent.height - topMargin - bottomMargin
    width: parent.width - leftMargin - rightMargin
    topMargin: guiSettings.headerMargin
    bottomMargin: guiSettings.footerMargin
    leftMargin: guiSettings.leftMargin
    rightMargin: guiSettings.rightMargin
    anchors.centerIn: parent

    header: RowLayout {
        Label {
            text: "Music generation"
            font.bold: true
            font.pointSize: 20
            Layout.fillWidth: true
            leftPadding: 25
        }
    }

    GridLayout {
        width: parent.width
        columns: 2

        CheckBox {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            text: "Generate music"
            checked: musicOn
            onCheckedChanged: musicOn = checked
        }

        Label {
            text: "Playing speed:"
        }
        SpinBox {
            from: minSpeed; to: maxSpeed
            editable: true
            value: speed
            enabled: musicOn
            onValueChanged: speed = value
        }

        Label {
            text: "Tone distance:"
        }
        SpinBox {
            from: minToneDistance; to: maxToneDistance
            editable: true
            value: toneDistance
            enabled: musicOn
            onValueChanged: toneDistance = value
        }

        Label {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: "For each line drawing circle piano tones are played at every \"tone distance\" pixels. " +
                  "Playing speed determines the speed of drawing and hence the speed of music playing."
        }
    }

    function show(scene) {
        musicOn = scene.musicGeneration
        speed = scene.playingSpeed
        minSpeed = scene.MIN_PLAYING_SPEED
        maxSpeed = scene.MAX_PLAYING_SPEED
        toneDistance = scene.toneDistance
        minToneDistance = scene.MIN_TONE_DISTANCE
        maxToneDistance = scene.MAX_TONE_DISTANCE
        open()
    }
}
