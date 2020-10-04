import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Jellyfin 1.0

import "qrc:/qml/Delegates"
import "qrc:/qml/Listers"
import "qrc:/qml/Player"

Page{
    id: mainPageStack

    property bool  showPlayer: false

    readonly property alias mediaPlayer: mainPlayer.mediaplayer

    anchors.fill: parent
    background: Item {}

    VerticalLister { anchors.fill: parent; visible: !showPlayer }

    MainPlayer { id: mainPlayer; anchors.fill: parent; visible: showPlayer }
}
