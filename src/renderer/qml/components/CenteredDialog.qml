import QtQuick.Controls 2.2

Dialog {
    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    modal: true

    closePolicy: Popup.CloseOnEscape

    standardButtons: Dialog.NoButton
}
