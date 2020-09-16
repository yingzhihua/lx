import QtQuick 2.0

Rectangle{
    property alias text1: message1.text
    property alias text2: message2.text
    property alias text3: message3.text
    width: 368
    height: 457
    radius: 22
    clip: true

    Image {
        source: "qrc:/images/ThreeMessageBG.png"
    }

    /*
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
    */
    Text{
        id: message1
        x:10
        y:20
        width: parent.width-20
        height: 140
        font.pixelSize: 40
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment:Text.AlignHCenter
        wrapMode: Text.Wrap
        clip:true
    }
    Text{
        id: message2
        x:10
        y:165
        width: parent.width-20
        height: 140
        font.pixelSize: 40
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment:Text.AlignHCenter
        wrapMode: Text.Wrap
        clip:true
    }
    Text{
        id: message3
        x:10
        y:310
        width: parent.width-20
        height: 140
        font.pixelSize: 40
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment:Text.AlignHCenter
        wrapMode: Text.Wrap
        clip:true
    }
}

