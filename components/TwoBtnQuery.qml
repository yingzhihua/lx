import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: promptDialog
    property alias msg: title.text
    property alias btn1: bt1.text
    property alias btn2: bt2.text
    signal closeAck(int res)

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 700
        height: 400
        anchors.centerIn: parent
        border.color: "darkgray"
        radius: 50

        Text{
            id:title
            y:100
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 40
        }

        Button {
            id: bt1
            text: qsTr("确定")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 70
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                promptDialog.visible = false;
                closeAck(1);
            }
        }

        Button {
            id: bt2
            text: qsTr("取消")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 70
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                promptDialog.visible = false;
                closeAck(2);
            }
        }
    }

    function show(){
        promptDialog.visible = true;
    }

    MouseArea{
        anchors.fill: parent
    }

}
