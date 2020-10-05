import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0
import Node 1.0

import "qrc:/qml/CustomComponents"

Item {

    height: 400
    ColumnLayout {
        anchors.fill: parent
        clip: true

        Rectangle {
            Layout.margins: 1
            Layout.preferredWidth: 219
            Layout.preferredHeight: 329
            Layout.alignment: Qt.AlignHCenter

            border.width: 1
            border.color: "#505050"

            gradient: Gradient {
                GradientStop { position: 0; color: "#303030" }
                GradientStop { position: 1; color: "#101010" }
            }

            Image {
                anchors.fill: parent
                anchors.margins: 1
                asynchronous: true
                source: modelData.getPrimaryImage(219, 329)
                sourceSize: Qt.size(219, 329)
                fillMode: Image.PreserveAspectCrop
            }
        }

        Text{
            color: "white"
            font.bold: true
            text: modelData.name
            Layout.preferredWidth: 219
            wrapMode: Text.Wrap
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
        }

        VLayoutSpacer{}
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
