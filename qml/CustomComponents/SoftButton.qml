import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
//import QtQuick.Controls.Material 2.12

Button {
    property alias gradient: bgRect.gradient

    id: control
    height: 35

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: "white"
    }

    background: Rectangle {
        id: bgRect
        implicitWidth: 40
        implicitHeight: control.height
        opacity: control.hovered ? control.down ? 0.9 : 0.6 : 0.0
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#505050" }
            GradientStop { position: 1.0; color: "#303030" }
        }
    }
}
