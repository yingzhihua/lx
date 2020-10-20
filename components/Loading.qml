import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Window 2.12

import QtQuick 2.0

Rectangle {
    id: lockDialog
    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)
    Rectangle{
        z:2
        width: 675
        height: 409
        anchors.centerIn: parent
        radius: 22

        Image {
            source: "qrc:/images/lockbg.png"
        }

        AnimatedImage{
            id:loadimg
            width: 230
            height: 230
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -50
            source: "qrc:/images/loading.gif"
            playing: false
        }

        Text{
            id: msg
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: loadimg.bottom
            anchors.topMargin: 30
            font.pixelSize: 50
        }
    }
    function show(msgtext){
        lockDialog.visible = true
        msg.text = msgtext
        loadimg.playing = true
    }
    function close(){
        loadimg.playing = false
        lockDialog.visible = false
    }

    MouseArea{
        anchors.fill: parent
    }
}

