import QtQuick
import QtWebView

Window {
    id: helpWindow
    width: root.width
    height: root.height
    color: "black"
    onClosing: destroy()

    WebView {
        id: web
        x: guiSettings.leftMargin
        y: guiSettings.headerMargin
        width: root.width - guiSettings.leftMargin - guiSettings.rightMargin
        height: root.height - guiSettings.headerMargin - guiSettings.footerMargin
    }

    function open() {
        web.url = "https://mfnboer.home.xs4all.nl/spiralfun/help/index.html?app=1"
        show()
    }

    function openMutationSequenceHelp() {
        web.url = "https://mfnboer.home.xs4all.nl/spiralfun/help/index.html?app=1#play_mutation_sequence"
        show()
    }
}
