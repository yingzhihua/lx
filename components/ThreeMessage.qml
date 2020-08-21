import QtQuick 2.0

Rectangle{
    property alias text1: message1.text
    property alias text2: message2.text
    property alias text3: message3.text
    width: 368
    height: 452
    radius: 22
    clip: true

    Rectangle{
        x: parent.width/2-11
        y: 11-parent.width/2
        width:22
        height: parent.width
        rotation: 90
        gradient: Gradient
        {
            GradientStop{position:0.0;color: "#cccccc"}
            GradientStop{position:1.0;color: "#e5e5e5"}
        }
    }
    Rectangle{
        x:8
        y:163
        width: parent.width-16
        height: 2
        color: "#a5a5a5"
    }
    Rectangle{
        x:8
        y:307
        width: parent.width-16
        height: 2
        color: "#a5a5a5"
    }

    Text{
        id: message1
        y:60
        font.pixelSize: 40
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Text{
        id: message2
        y:210
        font.pixelSize: 40
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Text{
        id: message3
        y:350
        font.pixelSize: 40
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

