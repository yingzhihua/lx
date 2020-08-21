import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: queryDialog
    signal queryAck(string res)

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 838
        height: 404
        anchors.centerIn: parent
        border.color: "darkgray"
        radius: 30

        Text{
            id:title
            y:115
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 60
        }

        Image {
            id: btQuit
            x: 67
            y: 277
            height: 84
            width: 214
            fillMode: Image.Pad
            source: "qrc:/images/cancelbt.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    queryDialog.visible = false;
                    queryAck("cancel")
                }
            }
        }

        Image {
            id: btConfirm
            x: 556
            y: 277
            height: 84
            width: 214
            fillMode: Image.Pad
            source: "qrc:/images/confirm.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    queryDialog.visible = false;
                    queryAck("confirm")
                }
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
