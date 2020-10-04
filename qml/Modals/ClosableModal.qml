import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "qrc:/qml/CustomComponents"

Popup {
    property int titleBarHeight: 35
    property string title: "Title placeholder"

    property alias content: modalContent.children

    id: popup
    padding: 0
    modal: true
    visible: false
    anchors.centerIn: parent
    height: contentItem.childrenRect.height

    contentItem:
        ColumnLayout {
            spacing: 0
            width: popup.width

            Rectangle {
                height: titleBarHeight
                Layout.fillWidth: true
                border.color: "#505050"
                border.width: 1

                gradient: Gradient {
                    GradientStop { position: 0; color: "#202020" }
                    GradientStop { position: 1; color: "#000000" }
                }

                RowLayout {
                    clip: true
                    anchors.fill: parent
                    Text {
                        Layout.leftMargin: 10
                        text: popup.title
                        color: "white"
                        font.bold: true
                    }
                    HLayoutSpacer{}
                    CloseButton{
                        width: titleBarHeight
                        height: titleBarHeight
                        onClicked: {
                            popup.close();
                            popup.destroy();
                        }
                    }
                }
            }

            Item {
                id: modalContent
                Layout.fillWidth: true
                Layout.margins: 10
                clip: true
                height: modalContent.childrenRect.height
            }

            VLayoutSpacer {}
        }

    background:
        Rectangle {
            anchors.fill: parent
            border.color: "#505050"
            border.width: 1
            gradient: Gradient {
                GradientStop { position: 0; color: "#202030" }
                GradientStop { position: 1; color: "#000010" }
            }
        }

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
}
