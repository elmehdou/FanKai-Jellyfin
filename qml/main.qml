import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0

import "qrc:/qml"

Window {
    id: root
    visible: true
    width: 1024
    height: 680
    title: qsTr("Fan Kai Jellyfin 1.0")

//    property string playerUrl: ""

    function moveToPage(page){ rootLoader.setSource(page) }

    function goToPlayer(playerUrl){
        if (String(rootLoader.source) !== "qrc:/qml/Pages/MainPage.qml"){
            rootLoader.setSource("qrc:/qml/Pages/MainPage.qml", {showPlayer: true});
        } else {
            rootLoader.item.showPlayer = true;
            rootLoader.item.mediaPlayer.play();
        }
        if (String(rootLoader.item.mediaPlayer.playerUrl) !== playerUrl){
            rootLoader.item.mediaPlayer.url = playerUrl;
//            rootLoader.item.mediaPlayer.time = Jellyfin.currentEpisode.positionTicks;
        }
    }

    function goToList(){
        if (String(rootLoader.source) !== "qrc:/qml/Pages/MainPage.qml"){
            rootLoader.setSource("qrc:/qml/Pages/MainPage.qml", {showPlayer: false});
        } else {
            rootLoader.item.showPlayer = false;
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0; color: "#101030" }
            GradientStop { position: 1; color: "#303050" }
        }
    }

    ColumnLayout{
        spacing: 0
        anchors.fill: parent

        Header{
            id: rootHeader
            height: 80
            Layout.preferredHeight: 80
            Layout.fillWidth: true
        }

        Loader {
            id: rootLoader
//            y: rootHeader.height
//            width: parent.width
//            height: 300
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "qrc:/qml/Pages/LoginPage.qml"
        }
    }

}
