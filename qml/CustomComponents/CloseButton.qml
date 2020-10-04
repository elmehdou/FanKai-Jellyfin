import QtQuick 2.0
import QtQuick.Controls 2.12

Button {
    property alias radius: bgRect.radius

    id: control
    width: 35
    height: 35
    contentItem: Text {
        text: "✕"
        color: "white"
        anchors.fill: control
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    background: Rectangle{
        id: bgRect
        implicitWidth: control.width
        implicitHeight: control.height
        border.color: "#505050"
        border.width: 1
        opacity: control.hovered ? control.down ? 0.35 : 1.0 : 0.65

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#f02020" }
            GradientStop { position: 1.0; color: "#a01010" }
        }
    }
}
