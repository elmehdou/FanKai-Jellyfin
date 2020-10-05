import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Jellyfin 1.0
import Node 1.0

import "qrc:/qml/Delegates"

ListView {

    function modelList(){
        if (!Jellyfin.currentNode) return [];
        var nodes = Jellyfin.currentNode.childrenNodes;

        if (parent.hasOwnProperty("keyword")){
            var keyword = parent.keyword
            if (keyword.length > 3) {
                nodes = nodes.filter( node => {
                                         if (!node.name) { return false }
                                         else if (node.name.toLower().contains(keyword.toLower())) { return true; }
                                     })
            }
        }


        nodes = nodes.sort((a,b) => {
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
        return nodes;
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

    populate: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 1000 }
    }
}
