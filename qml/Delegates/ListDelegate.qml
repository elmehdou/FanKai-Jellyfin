import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0
import Node 1.0

import "qrc:/qml/CustomComponents"

Rectangle {
    width: parent.width
    height: childrenRect.height

    border.width: 1
    border.color: "#505050"

    gradient: Gradient {
        GradientStop { position: 0; color: "#303030" }
        GradientStop { position: 1; color: "#101010" }
    }

    RowLayout {
        width: parent.width
        height: 60
        spacing: 0

        Image {
            Layout.preferredWidth: 180
            Layout.fillHeight:true
            Layout.margins: 1
            asynchronous: true
            source: modelData.getPrimaryImage(180,180)
            sourceSize: Qt.size(180, 180)
            fillMode: Image.PreserveAspectCrop
        }

        Text{
            color: "white"
            font.bold: true
            text: modelData.name
            Layout.fillWidth: true
            Layout.margins: 10
        }

        VLayoutSeparator {visible: modelData.indexNumber ? true : false}

        Text{
            color: "white"
            font.bold: true
            text: modelData.indexNumber ? modelData.indexNumber : ""
            Layout.preferredWidth: 50
            horizontalAlignment: Text.AlignHCenter
            visible: modelData.indexNumber ? true : false
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (modelData.type !== Node.Episode){
                modelData.updateChildren();
            } else {
                modelData.play();
            }

        }
    }
}
