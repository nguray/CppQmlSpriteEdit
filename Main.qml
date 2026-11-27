import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CppQmlSpriteEdit

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    title: "SpriteEdit"

    // 1. Menu Bar
    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Open")
                onTriggered: mainForm.ea.onMenuActionTriggered()
            }
            MenuSeparator{}
            Action {
                text: qsTr("&Save")
                onTriggered: mainForm.ea.onMenuActionTriggered()
            }
            Action {
                text: qsTr("Save &As...")
                onTriggered: mainForm.ea.onMenuActionTriggered()
            }
            MenuSeparator{}
            Action {
                text: qsTr("&Quit")
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: qsTr("&Edit")
            Action {
                text: qsTr("Undo")
                onTriggered: Qt.quit()
            }
            MenuSeparator{}
            Action {
                text: qsTr("Cut")
                shortcut: "Ctrl+X"
                onTriggered: mainForm.ea.onCutRectangle()
            }
            Action {
                text: qsTr("Copy")
                shortcut: "Ctrl+C"
                onTriggered: mainForm.ea.onCopyRectangle()
            }
            Action {
                text: qsTr("Paste")
                shortcut: "Ctrl+V"
                onTriggered: mainForm.ea.onPaste()
            }
        }

    }

    // 1. Create a non-visual ButtonGroup (exclusive by default)
    ButtonGroup {
        id: toolGroup
    }
    // Track the active tool globally in QML
    property string activeTool: "polyline"


    // Reusable custom ToolButton component for clean, consistent styling
    component DrawingToolButton: ToolButton {
        required property string toolId
        required property url iconPath

        icon.source: iconPath
        icon.width: 24
        icon.height: 24
        icon.color: "transparent"

        checkable: true
        ButtonGroup.group: toolGroup

        // 2. Custom highlight background when checked or hovered
        background: Rectangle {
            implicitWidth: 36
            implicitHeight: 36
            radius: 6
            color: parent.checked ? "#2196F333" : (parent.hovered ? "#80808022" : "transparent")
            border.color: parent.checked ? "#2196F3" : "transparent"
            border.width: 1.5
        }

        // Update activeTool the instant this button becomes checked
        onCheckedChanged: {
            if (checked) {
                root.activeTool = toolId
                console.log("Active tool is now:", root.activeTool)
            }
        }

    }

    // 2. Toolbar (assigned to the header property)
    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            DrawingToolButton {
                toolId: "rectangle_select"
                iconPath: "qrc:/icons/rectangle_select.png"
                onClicked: mainForm.ea.setSelectRectMode()
            }
            DrawingToolButton {
                toolId: "polyline"
                checked: true
                iconPath: "qrc:/icons/layer-shape-polyline.png"
                onClicked: mainForm.ea.setPencilMode()
            }
            DrawingToolButton {
                toolId: "rectangle"
                iconPath: "qrc:/icons/layer-shape.png"
                onClicked:  mainForm.ea.setRectangleMode()
            }
            DrawingToolButton {
                toolId: "ellipse"
                iconPath: "qrc:/icons/layer-shape-ellipse.png"
                onClicked: mainForm.ea.setEllipseMode()
            }
            DrawingToolButton {
                toolId: "flood_fill"
                iconPath: "qrc:/icons/flood-fill.png"
                onClicked: mainForm.ea.setFillMode()
            }
            Item { Layout.fillWidth: true } // Pushes remaining items to the right

        }
    }

    // 3. Main Form / Content Area (any direct child fills the remaining space)
    MainForm {
        id: mainForm
        anchors.fill: parent

        // Connections {
        //     target: mainForm.ea
        //     function onClicked() {
        //         console.log("Gauge clicked! New value:", mainForm.ea.value)
        //     }
        // }

        Connections {
            target: mainForm.pa
            // Answer to Signal colorPickerRequested from Palette Object
            function onColorPickerRequested() {
                // display ColorDialog
                mainForm.coldlg.selectedColor = mainForm.pa.selectColor
                console.log("ColorPickerDialog request: ",mainForm.pa.selectColor)
                mainForm.coldlg.open()
            }
        }

        Connections {
            target: mainForm.coldlg
            // Answer to Signal accepted from ColorDialog
            function onAccepted() {
                mainForm.pa.selectColor = mainForm.coldlg.selectedColor
                console.log("Select Color request: ",mainForm.pa.selectColor)
            }
        }

    }


    // 4. Status Bar (assigned to the footer property)
    footer: StatusBar {
        id: statusBar
        defaultText: "Ready"

        Connections {
            target: mainForm.ea

            function onStatusMessageRequested(message, timeout) {
                statusBar.showMessage(message, timeout)
            }

            function onStatusClearRequested() {
                statusBar.clearMessage()
            }
        }

        // Extra items placed here automatically appear on the right side
        // ToolSeparator {}
        // Label {
        //     text: "UTF-8"
        // }
    }

    Component.onCompleted: {
        console.log("Window and children are instantiated")

        // Wiring Signal/Slots between Palette and Editarea
        //mainForm.pa.foregroundColorChanged.connect(mainForm.ea.onForegroundColorChanged)

        // Call your initialization functions or C++ backend methods here
        //mainForm.pa.load(".","myPalette.txt");
    }

    Component.onDestruction: {
        console.log("Window is being destroyed")

        // Backuo current Palette colors
        mainForm.pa.save(".","myPalette.txt");

    }

}
