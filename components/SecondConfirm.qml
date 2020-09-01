import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: queryDialog
    signal queryAck(string res)

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 811
        height: 485
        anchors.centerIn: parent
        //border.color: "darkgray"
        radius: 22

        Image {
            anchors.fill: parent
            source: "qrc:/images/loginbk.png"
        }
        Text{
            id:title
            y:162
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 55
            font.bold: true
            color: "#323232"
        }

        Bt2{
            id: btQuit
            x:43
            y:354
            width: 255
            height: 85
            textoffsetx: 30
            textoffsety: -3
            textcolor: "#464646"
            text: qsTr("取消")
            image: "qrc:/images/cancel.png"
            onClicked: {
                queryDialog.visible = false;
                queryAck("cancel")
            }
        }
        Bt2{
            id: btConfirm
            x:519
            y:354
            width: 255
            height: 85
            textoffsetx: 30
            textoffsety: -3
            textcolor: "#ffffff"
            text: qsTr("确认")
            image: "qrc:/images/confirm.png"
            onClicked: {
                queryDialog.visible = false;
                queryAck("confirm")
            }
        }

    }

    function show(name){
        title.text = name;
        queryDialog.visible = true;
    }

    MouseArea{
        anchors.fill: parent
    }
}
