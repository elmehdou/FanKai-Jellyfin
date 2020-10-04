import QtQuick 2.12

ClosableModal {
    property string body: "Empty message body"
    id: modal
    width: 350
    height: 150

    content: Text {
        text: body
        color: "white"
        width: modal.width
        rightPadding: 15
        wrapMode: Text.WordWrap
    }

    Component.onCompleted: visible = true
}
