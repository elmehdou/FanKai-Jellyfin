import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Jellyfin 1.0
import Node 1.0

import "qrc:/qml/Delegates"

ListView {

    function modelList(){
        if (!Jellyfin.currentNode) return [];
        return Jellyfin.currentNode.childrenNodes;
    }

    id: rootList
    clip: true
    spacing: 0
    width: 0.75 * parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter

    model: modelList()

    header: ListHeaderDelegate {}

    delegate: ListDelegate {}
}
