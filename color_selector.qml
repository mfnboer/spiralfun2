import Qt.labs.platform

ColorDialog {
    onAccepted: destroy();
    onRejected: destroy();
}
