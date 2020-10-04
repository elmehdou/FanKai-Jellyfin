import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import VLCQt 1.1
import Jellyfin 1.0

import "qrc:/qml/CustomComponents"

Rectangle {
    readonly property alias mediaplayer: mediaPlayer

    color: "black"
    anchors.fill: parent

    VlcPlayer {
        id: mediaPlayer
        logLevel: Vlc.ErrorLevel
        onTimeChanged: {
            if (Math.abs(seekSlider.value - mediaPlayer.time) > 1000){
                seekSlider.value = mediaPlayer.time;
            }
        }
    }

    VlcVideoOutput {
        source: mediaPlayer
        anchors.fill: parent
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: ![Vlc.Playing, Vlc.Paused, Vlc.Stopped].includes(mediaPlayer.state) || mediaPlayer.time <= 0
        width: 150
        height: 150
        palette.dark: "white"
        opacity: 0.75
    }

    PlayerButton {
        source: "qrc:/images/close.svg"
        anchors.right: parent.right
        onClicked: function(){
            mediaPlayer.pause();
            root.goToList()
        }
    }

    RowLayout{
        spacing: 0
        width: parent.width
        anchors.bottom: parent.bottom

        PlayerButton {
            property var action : mediaPlayer.play
            id: playButton
            Layout.margins: 10
            source: mediaPlayer.state !== Vlc.Playing ? "qrc:/images/play.svg" : "qrc:/images/pause.svg"
            onClicked: function(mouse){
                if (mediaPlayer.state !== Vlc.Playing) mediaPlayer.play();
                else mediaPlayer.pause();
            }
        }

        PlayerButton {
            id: stopButton
            Layout.margins: 10
            source: "qrc:/images/stop.svg"
            onClicked: function(){ mediaPlayer.stop() }
        }

        Slider {
            id: seekSlider
            live: false
            Layout.margins: 10
            Layout.fillWidth: true
            to: mediaPlayer.length
            onValueChanged: {
                var newTime = seekSlider.value; // Multithreading is a bitch
                if (pressed) {
                    mediaPlayer.time = newTime;
                }
            }
        }

        Text{
            function formatTime(time) {
                var secs = (time / 1000) | 0;
                var mins = (secs / 60) | 0;
                var hours = (mins / 60) | 0;

                return hours + ":" + mins % 60 + ":" + secs %60;
            }

            text: formatTime(seekSlider.value) + "/" + formatTime(mediaPlayer.length)
            color: "white"
            opacity: 0.5
        }

        PlayerButton {
            id: subtitleButton
            source: "qrc:/images/closed-caption.svg"
            Layout.margins: 10

            onClicked: function(mouse){ contextSubtitleMenu.popup() }
            onPressAndHold: function(mouse){ if (mouse.source === Qt.MouseEventNotSynthesized) contextSubtitleMenu.popup() }

            Menu {
                id: contextSubtitleMenu
                width: 300
                Repeater {
                    model: mediaPlayer.subtitleTrackModel
                    delegate: MenuItem{
                        id: cSMenuItem
                        text: "Subtitle - " + model.title;
                        onClicked: {
                            mediaPlayer.subtitleTrack = model.id
                        }

                        background: Rectangle {
                            implicitWidth: 300
                            implicitHeight: 40
                            x: 1
                            y: 1
                            width: cSMenuItem.width - 2
                            height: cSMenuItem.height - 2
                            color: mediaPlayer.subtitleTrack === model.id ? "#aaaaff" : cSMenuItem.down ? "#8888ff" : cSMenuItem.highlighted ? "#ddddff" : "transparent"
                        }
                    }
                }
            }

        }

        PlayerButton {
            id: languageButton
            source: "qrc:/images/language.svg"
            Layout.margins: 10

            onClicked: function(mouse){ contextLanguageMenu.popup() }
            onPressAndHold: function(mouse){ if (mouse.source === Qt.MouseEventNotSynthesized) contextLanguageMenu.popup() }

            Menu {
                id: contextLanguageMenu
                width: 300
                Repeater {
                    model: mediaPlayer.audioTrackModel
                    delegate: MenuItem{
                        id: cLMenuItem
                        text: "Audio - " + model.title;
                        onClicked: {
                            mediaPlayer.audioTrack = model.id
                        }
                        background: Rectangle {
                            implicitWidth: 300
                            implicitHeight: 40
                            x: 1
                            y: 1
                            width: cLMenuItem.width - 2
                            height: cLMenuItem.height - 2
                            color: mediaPlayer.audioTrack === model.id ? "#aaaaff" : cLMenuItem.down ? "#8888ff" : cLMenuItem.highlighted ? "#ddddff" : "transparent"
                        }
                    }
                }
            }
        }
    }
}
