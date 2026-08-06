import QtQuick
import SpiralFun

Item {
    // Android geometry
    property int footerMargin: root.SafeArea.margins.bottom
    property int headerMargin: root.SafeArea.margins.top
    property int leftMargin: root.SafeArea.margins.left
    property int rightMargin: root.SafeArea.margins.right

    function openLink(link) {
        if (Qt.platform.os === "android")
            displayUtils.openLinkInApp(link)
        else
            Qt.openUrlExternally(link)
    }
}
