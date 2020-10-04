import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Jellyfin 1.0

import "qrc:/qml/Delegates"
import "qrc:/qml/Listers"
import "qrc:/qml/Player"

Page{
    id: mainPageStack

    property int viewType: rootHeader.viewType
    property bool showPlayer: false

    readonly property alias mediaPlayer: mainPlayer.mediaplayer

    anchors.fill: parent
    background: Item {}

    Loader {
        anchors.fill: parent;
        visible: !showPlayer
        source: viewType === 1 ? "qrc:/qml/Listers/GridLister.qml" : "qrc:/qml/Listers/VerticalLister.qml"
    }

    MainPlayer { id: mainPlayer; anchors.fill: parent; visible: showPlayer }
}
