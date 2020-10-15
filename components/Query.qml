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
        width: 799
        height: 469
        anchors.centerIn: parent
        //border.color: "darkgray"
        radius: 22

        Image {
            anchors.fill: parent
            source: "qrc:/images/wifibg.png"
        }

        Label{
            id:title
            anchors.horizontalCenter: parent.horizontalCenter
            y:18
            font.pixelSize: 45
        }

        Text{
            id:ssid
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: title.bottom
            anchors.topMargin: 70
            font.pixelSize: 40
        }

        Text{
            id:queryLable
            x:150
            width: 100
            anchors.top: ssid.bottom
            font.pixelSize: 40
            anchors.topMargin: 40
        }

        TextField{
            id: ack
            anchors.left: queryLable.right
            anchors.leftMargin: 20
            anchors.verticalCenter: queryLable.verticalCenter
            width: 400
            height: 64
            echoMode: TextInput.Password
            background: Rectangle{
                border.color: "#cdcdcd"
                color: "#f5f5f5"
            }
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
                focus = true
                queryDialog.visible = false;                
            }
        }
        Bt2{
            id: btSet
            x:500
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
                focus = true
                queryDialog.queryAck(ack.text);
            }
        }

    }

    function show(name){
        ssid.text = name;
        ack.text = "";
        queryDialog.visible = true;
    }

    MouseArea{
        anchors.fill: parent
    }
}
