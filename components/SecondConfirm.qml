import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: queryDialog
    signal queryAck(string res)

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
            id: btConfirm
            text: qsTr("确定")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                queryDialog.visible = false;
                queryAck("confirm")
            }
        }

        Button {
            id: btQuit
            text: qsTr("取消")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                queryDialog.visible = false;
                queryAck("cancel")
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
