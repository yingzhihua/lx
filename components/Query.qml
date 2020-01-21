import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: queryDialog
    property alias qtitle: title.text
    property alias qlable: queryLable.text
    property alias qbutton: btSet.text
    signal queryAck(string res)

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
            id:ssid
            x:50
            anchors.top: title.bottom
            anchors.topMargin: 50
            text: ""
        }

        Text{
            id:queryLable
            x:50
            width: 100
            anchors.top: ssid.bottom
            anchors.topMargin: 60
        }

        TextField{
            id: ack
            anchors.left: queryLable.right
            anchors.leftMargin: 50
            anchors.verticalCenter: queryLable.verticalCenter
            width: 400
        }

        Button {
            id: btSet
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 70
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -150
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                queryDialog.visible = false;
                queryDialog.queryAck(ack.text);
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

    function show(name){
        ssid.text = name;
        queryDialog.visible = true;
    }

    MouseArea{
        anchors.fill: parent
    }
}
