import QtQuick
import QtQuick.Layouts
import CppQmlSpriteEdit

ColumnLayout {

    id: rectangle
    anchors.fill: parent
    spacing: 0

    property alias ea: editarea

    EditArea {
        id: editarea
        objectName: "myeditarea" // Required so C++ can find it
        value: 75
        Layout.fillHeight: true // Expands to take up all leftover space
        Layout.fillWidth: true
    }

    Palette {
        id: palette
        objectName: "mypalette" // Required so C++ can find it
        Layout.fillWidth: true
        Layout.preferredHeight: 34 // Sets the fixed height
        // Optional: lock min/max so it never resizes even if space is tight
        Layout.minimumHeight: 34
        Layout.maximumHeight: 34
    }
}
