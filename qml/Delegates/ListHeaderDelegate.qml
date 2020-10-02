import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0
import Node 1.0

Rectangle {
    width: parent.width
    height: Jellyfin.currentNode ? Jellyfin.currentNode.parentNode ? 65 : 0 : 0
    visible: Jellyfin.currentNode ? Jellyfin.currentNode.parentNode ? true : false : false

    border.width: 1
    border.color: "#505050"

    gradient: Gradient {
        GradientStop { position: 0; color: "#202020" }
        GradientStop { position: 1; color: "#000000" }
    }

    Image {
        anchors.fill: parent
        anchors.margins: 1
        asynchronous: true
        source: Jellyfin.currentNode ?  Jellyfin.currentNode.getPrimaryImage(width, width) : ""
        sourceSize: Qt.size(width, width)
        fillMode: Image.PreserveAspectCrop
    }

    Button{
        id: control
        anchors.margins: 1
        height: parent.height
        width: 300
        font.bold: true
        palette.buttonText: "white"
        icon.source: "qrc:/images/backarrow.svg"
        icon.color: "transparent"
        text: Jellyfin.currentNode ? Jellyfin.currentNode.parentNode ? Jellyfin.currentNode.parentNode.name : "" : ""
        background: Rectangle {
            implicitWidth: control.width
            implicitHeight: control.height
            color: "black"
            opacity: 0.50
            border.width: 1
            border.color: "#505050"
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (Jellyfin.currentNode.parentNode){
                Jellyfin.currentNode = Jellyfin.currentNode.parentNode;
            }
        }
    }
}
