// StatusBar.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ToolBar {
    id: root
    height: 28

    // The fallback text shown when no temporary message is active
    property string defaultText: "Ready"

    // Expose the layout so you can add extra right-side widgets (like progress bars)
    default property alias content: rowLayout.data

    // Replicates QStatusBar::showMessage(message, timeout)
    // Pass timeout = 0 (or omit) for a persistent message until cleared
    function showMessage(message, timeout = 0) {
        statusLabel.text = message
        if (timeout > 0) {
            clearTimer.interval = timeout
            clearTimer.restart()
        } else {
            clearTimer.stop()
        }
    }

    // Replicates QStatusBar::clearMessage()
    function clearMessage() {
        clearTimer.stop()
        statusLabel.text = root.defaultText
    }

    Timer {
        id: clearTimer
        repeat: false
        onTriggered: root.clearMessage()
    }

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        Label {
            id: statusLabel
            text: root.defaultText
            elide: Text.ElideRight
            Layout.fillWidth: true
        }
    }
}
