import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: promptDialog
    signal closeAck()

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
            text: qsTr("关闭")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
            anchors.horizontalCenter: parent.horizontalCenter
            width: 200
            height: 80
            font.pixelSize: 40
            onClicked: {
                promptDialog.visible = false;
                closeTimer.stop();
                closeAck()
            }
        }
    }

    function show(name){
        console.log("AutoCloseMsg"+name);
        title.text = name;
        promptDialog.visible = true;
        closeTimer.start();
    }

    MouseArea{
        anchors.fill: parent
    }

    Timer{
        id: closeTimer
        interval: 30000
        running: false
        onTriggered: {
            promptDialog.visible = false;
            closeAck();
        }
    }
}

