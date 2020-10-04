import QtQuick 2.12
import QtQuick.Layouts 1.12
import "qrc:/qml/CustomComponents"

Rectangle{
    property int titleBarHeight: 35
    property string title: "Empty message body"
    property string body: "Empty message body"

    id: notificationRect
    width: 400
    height: 100
    border.color: "#505050"
    border.width: 1
    clip: true
    gradient: Gradient {
        GradientStop { position: 0; color: "#202020" }
        GradientStop { position: 1; color: "#000000" }
    }

    function show(){
        onCompletedAnimation.start();
    }

    function close(){
        if (!onDestructionAnimation.running)
            onDestructionAnimation.start();
    }

    ColumnLayout {
        spacing: 0
        width: parent.width

        Rectangle {
            height: titleBarHeight
            Layout.fillWidth: true
            border.color: "#505050"
            border.width: 1

            gradient: Gradient {
                GradientStop { position: 0; color: "#202030" }
                GradientStop { position: 1; color: "#000010" }
            }

            RowLayout {
                clip: true
                anchors.fill: parent
                Text {
                    Layout.leftMargin: 10
                    text: notificationRect.title
                    color: "white"
                    font.bold: true
                }
                HLayoutSpacer{}
                CloseButton{
                    width: titleBarHeight
                    height: titleBarHeight
                    onClicked: close();
                }
            }
        }
//        VLSpacer {}

        Text {
            text: body
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"
            width: notificationRect.width
            padding: 15
            wrapMode: Text.WordWrap
        }
    }

    SequentialAnimation {
        id: onCompletedAnimation
        ScriptAction{script: notificationRect.visible = true}
        ParallelAnimation {
            NumberAnimation {
                target: notificationRect
                property: "opacity"
                duration: 250
                easing.type: Easing.InOutQuad
                from: 0.0
                to: 1.0
            }
            NumberAnimation {
                target: notificationRect
                property: "x"
                duration: 500
                easing.type: Easing.InOutQuad
                from: root.width
                to: 0
            }
        }
        ScriptAction { script: timer.start(); }
    }

    Timer {
        id: timer
        interval: 5000;
        onTriggered: close()
    }

    SequentialAnimation {
        id: onDestructionAnimation
        ParallelAnimation {
            NumberAnimation {
                target: notificationRect
                property: "opacity"
                duration: 250
                easing.type: Easing.InOutQuad
                to: 0.0
            }
            NumberAnimation {
                target: notificationRect
                property: "x"
                duration: 500
                easing.type: Easing.InOutQuad
                to: notificationRect.width
            }
        }
        ScriptAction{script: notificationRect.visible = false}
        ScriptAction{script: notificationRect.destroy()}
    }

    Component.onCompleted: {
        show();
    }
}
