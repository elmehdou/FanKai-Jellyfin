import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Jellyfin 1.0
import QmlState 1.0

import "qrc:/qml/Delegates"
import "qrc:/qml/Listers"
import "qrc:/qml/Player"

Page{
    id: mainPageStack

    property string keyword: ""

    anchors.fill: parent
    background: Item {}

    Loader {
        anchors.fill: parent;
        visible: !QmlState.playerShow;
        source: QmlState.viewType === 1 ? "qrc:/qml/Listers/GridLister.qml" : "qrc:/qml/Listers/VerticalLister.qml"
    }

    MainPlayer {
        id: mainPlayer;
        anchors.fill: parent;
        visible: QmlState.playerShow;
    }
}
