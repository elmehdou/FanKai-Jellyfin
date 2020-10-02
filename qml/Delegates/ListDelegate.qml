import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0
import Node 1.0

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
