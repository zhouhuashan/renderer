import QtQml 2.2

import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.3

import Qt.labs.settings 1.0

import Renderer 1.0
import Clipboard 1.0

ApplicationWindow {
    id: root

    objectName: Qt.application.name

    visible: true
    visibility: Window.AutomaticVisibility

    //minimumWidth: 64
    //minimumHeight: 64

    //maximumWidth: screen.desktopAvailableWidth
    //maximumHeight: screen.desktopAvailableHeight

    //width: 512
    //height: 512

    Component.onCompleted: {
        //x = (screen.width - width) / 2
        //y = (screen.height - height) / 2
    }

    Shortcut {
        sequence: StandardKey.Cancel // "Escape"
        autoRepeat: false
        context: Qt.ApplicationShortcut
        onActivated: {
            if (visibility === Window.FullScreen) {
                show()
            } else {
                close()
            }
        }
    }

    CenteredDialog {
        id: confirmationDialog

        title: qsTr("Close application")

        Label {
            anchors.fill: parent
            text: qsTr("Are you sure?")
        }

        standardButtons: Dialog.Yes | Dialog.No

        onAccepted: root.close()
    }

    onClosing: {
        if (visibility === Window.FullScreen) {
            show()
            confirmationDialog.open()
            close.accepted = false
        }
    }

    Shortcut {
        sequence: StandardKey.FullScreen // "F11"
        autoRepeat: false
        onActivated: {
            if (visibility === Window.FullScreen) {
                root.show()
            } else {
                root.showFullScreen()
            }
        }
    }

    CenteredDialog {
        id: settingsDialog

        title: qsTr("Settiings")

        RowLayout {
            anchors.fill: parent

            Button {
                text: qsTr("Refresh")
                onClicked: renderItem.update()
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            }
            CheckBox {
                text: qsTr("Autorefresh")
                onCheckedChanged: {
                    renderItem.renderer.autoRefresh = checked
                }
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            }
            Button {
                text: qsTr("Copy content")
                onClicked: renderItem.grabToClipboard()
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            }
            Item {
                Layout.fillWidth: true
            }
        }

        standardButtons: Dialog.Ok | Dialog.Apply | Dialog.Cancel

        onAccepted: {

        }
        onRejected: {

        }
    }

    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Settings dialog...")

            onTriggered: {
                settingsDialog.open()
            }
        }
        MenuItem {
            text: qsTr("Copy content")

            onTriggered: {
                renderItem.grabToClipboard()
            }
        }
        MenuItem {
            text: qsTr("Action")

            onTriggered: {
                renderItem.source = "file:///home/user2/rbin_with_ropes/NorthKoreanNuclearTestSite_08082017_Leica3Dscan_N317.rbin"
            }
        }
        MenuItem {
            text: qsTr("Action 2")

            onTriggered: {
                renderItem.source = ""
            }
        }
    }

    RenderItem {
        id: renderItem

        objectName: "renderItem"

        anchors.fill: parent

        function grabToClipboard() {
            if (!renderItem.grabToImage(function(grabResult) {
                Clipboard.setImage(grabResult.image)
                console.log("Image with URL '%1' copied to clipboard".arg(grabResult.url))
                gc()
            })) {
                console.log("unable to grab renderItem to Image")
            }
        }

        camera.aspectRatio: Math.max(1, width) / Math.max(1, height)

        focus: true

        MouseArea {
            anchors.fill: parent

            cursorShape: parent.cursor

            acceptedButtons: Qt.RightButton

            onClicked: {
                if (mouse.button === Qt.RightButton) {
                    contextMenu.x = mouse.x
                    contextMenu.y = mouse.y
                    contextMenu.open()
                    mouse.accepted = true
                }
            }
        }
    }

    title: qsTr("%1 FPS = %2").arg(Qt.application.displayName).arg(1.0 / renderItem.renderer.dt)

    Shortcut {
        sequence: "F4"
        autoRepeat: false
        onActivated: renderItem.renderer.autoRefresh = false
    }

    Shortcut {
        sequence: "F5"
        autoRepeat: false
        onActivated: renderItem.renderer.autoRefresh = true
    }

    Shortcut {
        sequence: StandardKey.Refresh // "Ctrl+R"
        onActivated: renderItem.update()
    }

    Shortcut {
        sequence: StandardKey.Copy // "Ctrl+C"
        autoRepeat: false
        onActivated: renderItem.grabToClipboard()
    }

    Shortcut {
        sequence: StandardKey.Preferences // "Ctrl+,"?
        autoRepeat: false
        onActivated: settingsDialog.open()
    }

    Settings {
        property alias rootVisibility: root.visibility

        property alias rootWidth: root.width
        property alias rootHeight: root.height

        property alias rootX: root.x
        property alias rootY: root.y

        property alias source: renderItem.source
    }
}
