import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0

import "qrc:/qml/CustomComponents"

Rectangle {

    height: 80


    gradient: Gradient {
        GradientStop { position: 0; color: "#000030" }
        GradientStop { position: 0.99; color: "#000000" }
        GradientStop { position: 1; color: "#ffffff" }
    }

    RowLayout {
        anchors.fill: parent

        Image {
            height: 80
            source: "qrc:/images/logo.png"
            sourceSize.height: 80
            fillMode: Image.PreserveAspectFit
        }

        VLayoutSeparator {Layout.margins: 5}

        Text {
            color: "white"
            font.bold: true
            text: Jellyfin.currentNode ? Jellyfin.currentNode.name : ""
            Layout.fillHeight: true
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            fontSizeMode: Text.Fit
            minimumPixelSize: 10
            font.pixelSize: 35
        }

        BusyIndicator {
            id: indicator
            running: Jellyfin.working
            palette.dark: "white"
            opacity: 0.75
        }
    }
}