import QtQuick 2.0

Image {
    property var onClicked: undefined
    property var onPressAndHold: undefined

    id: control
    width: 35
    height: 35
    opacity: 0.5
    sourceSize: Qt.size(35, 35)

    MouseArea {
        id: mouseArea
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {if (control.onClicked) control.onClicked(mouse);}
        onPressAndHold: {if (control.onPressAndHold) control.onPressAndHold(mouse);}
        onContainsMouseChanged: {
            if (containsMouse) parent.opacity = 0.75
            else parent.opacity = 0.5
        }
    }
}
