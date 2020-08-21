import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: queryDialog
    property alias qtitle: title.text
    property alias qlable1: query1Lable.text
    property alias qlable2: query2Lable.text
    property alias qlable3: query3Lable.text
    property alias qtext1: ack1.text
    property alias qtext2: ack2.text
    property alias qtext3: ack3.text

    signal queryAck(string res1, string res2, string res3)

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 838
        height: 561
        anchors.centerIn: parent
        border.color: "darkgray"
        radius: 22

        Text{
            id:title
            y:40
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 50
        }

        Text{
            id:query1Lable
            x:100
            width: 100
            anchors.top: title.bottom
            anchors.topMargin: 50
            font.pixelSize: 40
            color: "#a7a7a7"
        }

        TextField{
            id: ack1
            anchors.left: query1Lable.right
            anchors.leftMargin: 100
            anchors.verticalCenter: query1Lable.verticalCenter
            width: 400
        }

        Text{
            id:query2Lable
            width: 100
            anchors.left: query1Lable.left
            anchors.leftMargin: 0
            anchors.top: query1Lable.bottom
            anchors.topMargin: 30
            font.pixelSize: 40
            color: "#a7a7a7"
        }

        TextField{
            id: ack2
            anchors.horizontalCenter: ack1.horizontalCenter
            anchors.verticalCenter: query2Lable.verticalCenter
            width: 400
        }

        Text{
            id:query3Lable
            anchors.left: query1Lable.left
            anchors.leftMargin: 0
            width: 100
            anchors.top: query2Lable.bottom
            anchors.topMargin: 30
            font.pixelSize: 40
            color: "#a7a7a7"
        }

        TextField{
            id: ack3
            anchors.horizontalCenter: ack1.horizontalCenter
            anchors.verticalCenter: query3Lable.verticalCenter
            width: 400
        }

        Image {
            id: btQuit
            x: 68
            y: 448
            height: 84
            width: 214
            fillMode: Image.Pad
            source: "qrc:/images/cancelbt.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    queryDialog.visible = false;
                }
            }
        }

        Image {
            id: btConfirm
            x: 557
            y: 448
            height: 84
            width: 214
            fillMode: Image.Pad
            source: "qrc:/images/confirm.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    queryDialog.visible = false;
                    queryDialog.queryAck(ack1.text,ack2.text,ack3.text);
                }
            }
        }
    }

    function show(){
        queryDialog.visible = true;
    }

    MouseArea{
        anchors.fill: parent
    }
}
