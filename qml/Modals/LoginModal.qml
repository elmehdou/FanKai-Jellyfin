import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Jellyfin 1.0

Rectangle {
    width: 400
    height: childrenRect.height
    anchors.centerIn: parent

    border.width: 1
    border.color: "#505050"

    gradient: Gradient {
        GradientStop { position: 0; color: "#202020" }
        GradientStop { position: 1; color: "#000000" }
    }

    ColumnLayout{
        spacing: 0
        width: parent.width

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50

            border.width: 1
            border.color: "#505050"

            gradient: Gradient {
                GradientStop { position: 0; color: "#202030" }
                GradientStop { position: 1; color: "#000010" }
            }

            Text{
                text: "Bienvenue sur Fan-kai !"
                color: "white"
                anchors.centerIn: parent
            }

        }

        TextField{
            id: usernameField
            placeholderText: "Username"
            Layout.fillWidth: true
            Layout.margins: 10
        }

        TextField{
            id: passwordField
            placeholderText: "Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.margins: 10
        }

        Button {
            text: "Log in"
            Layout.fillWidth: true
            Layout.margins: 10
            onClicked: Jellyfin.login(usernameField.text, passwordField.text , false)
        }
    }
}
