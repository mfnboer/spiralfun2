import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebView

Window {
    id: helpWindow
    onClosing: destroy()

    WebView {
        id: web
        anchors.fill: parent
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
