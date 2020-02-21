import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: queryDialog
    property alias qtitle: title.text
    property alias qlable1: query1Lable.text
    property alias qlable2: query2Lable.text
    property alias qtext1: ack1.text
    property alias qtext2: ack2.text

    signal queryAck(string res1, string res2)

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 800
        height: 500
        anchors.centerIn: parent
        border.color: "darkgray"
        radius: 50

        Text{
            id:title
            y:50
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 40
        }

        Text{
            id:query1Lable
            x:50
            width: 100
            anchors.top: title.bottom
            anchors.topMargin: 50
        }

        TextField{
            id: ack1
            anchors.left: query1Lable.right
            anchors.leftMargin: 50
            anchors.verticalCenter: query1Lable.verticalCenter
            width: 400
        }

        Text{
            id:query2Lable
            x:50
            width: 100
            anchors.top: query1Lable.bottom
            anchors.topMargin: 60
        }

        TextField{
            id: ack2
            anchors.left: query2Lable.right
            anchors.leftMargin: 50
            anchors.verticalCenter: query2Lable.verticalCenter
            width: 400
        }

        Button {
            id: btSet
            text: qsTr("确定")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 70
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                queryDialog.visible = false;
                queryDialog.queryAck(ack1.text,ack2.text);
            }
        }

        Button {
            id: btQuit
            text: qsTr("取消")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 70
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                queryDialog.visible = false;
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

