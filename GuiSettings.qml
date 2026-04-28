import QtQuick
import SpiralFun

Item {
    // Android geometry
    property int footerMargin: getNavigationBarSize(QEnums.INSETS_SIDE_BOTTOM)
    property int headerMargin: getStatusBarSize(QEnums.INSETS_SIDE_TOP)
    property int leftMargin: Math.max(getNavigationBarSize(QEnums.INSETS_SIDE_LEFT), getDisplayCutoutSize(QEnums.INSETS_SIDE_LEFT))
    property int rightMargin: Math.max(getNavigationBarSize(QEnums.INSETS_SIDE_RIGHT), getDisplayCutoutSize(QEnums.INSETS_SIDE_RIGHT))

    function updateScreenMargins() {
        footerMargin = getNavigationBarSize(QEnums.INSETS_SIDE_BOTTOM)
        headerMargin = getStatusBarSize(QEnums.INSETS_SIDE_TOP)
        leftMargin = Math.max(getNavigationBarSize(QEnums.INSETS_SIDE_LEFT), getDisplayCutoutSize(QEnums.INSETS_SIDE_LEFT))
        rightMargin = Math.max(getNavigationBarSize(QEnums.INSETS_SIDE_RIGHT), getDisplayCutoutSize(QEnums.INSETS_SIDE_RIGHT))

        console.debug("MARGINS L:", leftMargin, "R:", rightMargin, "T:", headerMargin, "B:", footerMargin)
    }

    function getNavigationBarSize(side) {
        return displayUtils.getNavigationBarSize(side) / Screen.devicePixelRatio
    }

    function getStatusBarSize(side) {
        return displayUtils.getStatusBarSize(side) / Screen.devicePixelRatio
    }

    function getDisplayCutoutSize(side) {
        return displayUtils.getDisplayCutoutSize(side) / Screen.devicePixelRatio
    }

    function openLink(link) {
        if (Qt.platform.os === "android")
            displayUtils.openLinkInApp(link)
        else
            Qt.openUrlExternally(link)
    }
}
