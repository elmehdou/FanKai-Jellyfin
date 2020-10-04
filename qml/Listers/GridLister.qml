import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Jellyfin 1.0
import Node 1.0

import "qrc:/qml/Delegates"

GridView {

    function modelList(){
        if (!Jellyfin.currentNode) return [];
        return Jellyfin.currentNode.childrenNodes.sort((a,b) => {
                                                           if (a.hasOwnProperty("indexNumber")){
                                                               return a.indexNumber - b.indexNumber;
                                                           } else {
                                                               var fa = a.name.toLowerCase();
                                                               var fb = b.name.toLowerCase();

                                                               if (fa < fb) { return -1; }
                                                               if (fa > fb) { return 1; }
                                                               return 0;
                                                           }
                                                       });
    }

    id: rootList
    clip: true
    cellWidth: 250
    cellHeight: 400
    width: 0.75 * parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter

    model: modelList()

    header: ListHeaderDelegate {}

    delegate: GridDelegate {}

    populate: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 1000 }
    }
}
