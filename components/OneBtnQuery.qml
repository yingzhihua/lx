import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: promptDialog
    property alias bttext: backbt.text
    property alias btimage: backbt.image
    property alias btwidth: backbt.width
    property alias btheight: backbt.height
    signal closeAck()

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 811
        height: 485
        anchors.centerIn: parent
        radius: 22

        Image {
            anchors.fill: parent
            source: "qrc:/images/loginbk.png"
        }

        Text{
            id:title
            y:165
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 50
            font.bold: true
            color: "#323232"
        }

        Bt2{
            id: backbt
            anchors.horizontalCenter: parent.horizontalCenter
            y:355
            width: 280
            height: 85
            textoffsetx: 30
            textoffsety: -3
            textcolor: "#ffffff"
            text: qsTr("取消测试")
            image: "qrc:/images/cancelTestbt.png"
            onClicked: {
                promptDialog.visible = false
                closeAck()
            }
        }
    }

    function show(msg){
        promptDialog.visible = true;
        title.text = msg;
    }

    MouseArea{
        anchors.fill: parent
    }

}
