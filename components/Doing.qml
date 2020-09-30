import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
    property int timecount: 0

    id: promptDialog
    signal closeAck()

    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 798
        height: 375
        anchors.centerIn: parent        
        radius: 22

        Image {
            anchors.fill: parent
            source: "qrc:/images/upasswdbg.png"
        }

        Text{
            id:title
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -50
            anchors.horizontalCenterOffset: -50
            font.pixelSize: 45
        }
        Label{
            id:title_end
            anchors.left: title.right
            anchors.leftMargin: 10
            anchors.verticalCenter: title.verticalCenter
        }

        Bt2{
            id: btQuit
            anchors.top: title.bottom
            anchors.topMargin: 70
            anchors.horizontalCenter: parent.horizontalCenter
            width: 255
            height: 85
            textoffsetx: 30
            textoffsety: -3
            textcolor: "#464646"
            text: qsTr("取消")
            image: "qrc:/images/cancel.png"
            onClicked: {
                closeAck()
                changeTimer.stop()
            }
        }
    }

    function show(name){        
        title.text=name
        promptDialog.visible = true;
        changeTimer.start()
    }

    function close(){
        promptDialog.visible = false;
        changeTimer.stop()
    }

    MouseArea{
        anchors.fill: parent
    }

    Timer{
        id: changeTimer
        repeat: true
        interval: 500
        running: false
        onTriggered: {
            timecount++;
            if (timecount > 5)
                timecount = 0;
            if(timecount == 0)
                title_end.text="。";
            else if(timecount == 1)
                title_end.text="。。";
            else if(timecount == 2)
                title_end.text="。。。";
            else if(timecount == 3)
                title_end.text="。。。。";
            else
                title_end.text="。。。。。";
        }
    }
}
