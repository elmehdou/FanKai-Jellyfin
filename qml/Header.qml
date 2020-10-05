import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0
import QmlState 1.0

import "qrc:/qml/CustomComponents"

Rectangle {

    height: 80
    visible: !QmlState.playerShow

    gradient: Gradient {
        GradientStop { position: 0; color: "#000030" }
        GradientStop { position: 0.99; color: "#000000" }
        GradientStop { position: 1; color: "#ffffff" }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        Image {
            height: 80
            source: "qrc:/images/logo.png"
            sourceSize.height: 80
            fillMode: Image.PreserveAspectFit
        }

        VLayoutSeparator {Layout.margins: 5}

        ColumnLayout{
            spacing: 0
            Layout.margins: 5
            Layout.fillWidth: true
            Layout.fillHeight: true
            Text {
                color: "white"
                font.bold: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: Jellyfin.currentNode ? Jellyfin.currentNode.name : ""
                verticalAlignment: Text.AlignVCenter
                fontSizeMode: Text.Fit
                minimumPixelSize: 10
                font.pixelSize: 20
            }
            RowLayout {
                SoftButton {
                    height: 20
                    visible: Jellyfin.currentSeries ? true : false;
                    text: Jellyfin.currentSeries ? Jellyfin.currentSeries.name : ""
                    onClicked: {
                        Jellyfin.currentNode = Jellyfin.currentSeries
                        Qt.rootWindow.goToList();
                    }
                }
                Text {
                    visible: Jellyfin.currentSeason ? true : false;
                    text: ">"
                    color: "white"
                    font.bold: true
                }
                SoftButton {
                    height: 20
                    visible: Jellyfin.currentSeason ? true : false;
                    text: Jellyfin.currentSeason ? Jellyfin.currentSeason.name : ""
                    onClicked: {
                        Jellyfin.currentNode = Jellyfin.currentSeason
                        Qt.rootWindow.goToList();
                    }
                }
                Text {
                    visible: Jellyfin.currentEpisode ? true : false;
                    text: ">"
                    color: "white"
                    font.bold: true
                }
                SoftButton {
                    height: 20
                    visible: Jellyfin.currentEpisode ? true : false;
                    text: Jellyfin.currentEpisode ? Jellyfin.currentEpisode.name : ""
                    onClicked: {
                        Jellyfin.currentNode = Jellyfin.currentSeason
                        Qt.rootWindow.goToPlayer();
                    }
                }
            }
        }

        HLayoutSpacer {}

        BusyIndicator {
            id: indicator
            running: Jellyfin.working
            palette.dark: "white"
            opacity: 0.75
        }

        VLayoutSeparator{}

        SoftButton {
            Layout.fillHeight: true
            icon.source: QmlState.viewType === QmlState.ListView ? "qrc:/images/grid.svg" : "qrc:/images/list.svg"
            icon.color: "white"
            display: AbstractButton.IconOnly
            onClicked: {
                if (QmlState.viewType === QmlState.ListView) QmlState.viewType = QmlState.GridView
                else if (QmlState.viewType === QmlState.GridView) QmlState.viewType = QmlState.ListView
            }
        }
    }
}
