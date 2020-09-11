import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Sequence 1.0
import Dx.Global 1.0
/*
Item {
    Text{
        id: labelLock
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -100
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -100
        width: 80
        height: 30
        text: qsTr("锁屏时间：")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
    }

    SpinBox{
        id:spinLock
        anchors.left: labelLock.right
        anchors.leftMargin: 20
        anchors.verticalCenter: labelLock.verticalCenter
        from:0
        value: 30
        to:1000
        stepSize: 10
        font.pixelSize: 40
        width: 300
        editable: false
        contentItem: TextInput{
            text:spinLock.value+" S"
            horizontalAlignment: Qt.AlignHCenter
            readOnly: !spinLock.editable
        }
        up.indicator: Rectangle{
            x:parent.width - width
            height: parent.height
            implicitWidth: 60
            color: spinLock.up.pressed?"#f6f6f6":"#e4e4e4"
            //border.color: enabled?"#21be2b":"#bdbebf"
            Text{
                text:"+"
                font.pixelSize: spinLock.font.pixelSize*2
                //color: "#21be2b"
                anchors.fill: parent
                fontSizeMode: Text.Fit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        down.indicator: Rectangle{
            x:0
            height: parent.height
            implicitWidth: 60
            color: spinLock.down.pressed?"#f6f6f6":"#e4e4e4"
            //border.color: enabled?"#21be2b":"#bdbebf"
            Text{
                text:"-"
                font.pixelSize: spinLock.font.pixelSize*2
                //color: "#21be2b"
                anchors.fill: parent
                fontSizeMode: Text.Fit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }        
    }


    Button{
        id: btSave
        anchors.top: labelLock.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: labelLock.horizontalCenter
        anchors.horizontalCenterOffset: 100
        width: 150
        height: 50

        contentItem: Text {
            text: qsTr("保存")
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            font.pixelSize: 30
        }
        background: Rectangle{
            color: "darkgray"
            radius: 5
        }
        onPressed: {
        //onClicked: {
            console.log(spinLock.value);
            ExGlobal.lockTime = spinLock.value;
            mainView.pop();
        }
    }
*/

Rectangle{
    //property bool lockscreenopen: false
    //anchors.fill: parent
    color: "#f5f5f5"

    Rectangle{
        width: 584
        height: 301
        radius: 22
        anchors.centerIn: parent

        Image {
            anchors.fill: parent
            source: "qrc:/images/lockscreenbg.png"
        }

        Text {
            text: qsTr("启动锁屏功能")
            x:49
            y:15
            font.pixelSize: 40
        }

        Image {
            id: lockscreenswitch
            x:447
            y:22
            source: ExGlobal.lockscreenOpen?"qrc:/images/lockscreenON.png":"qrc:/images/lockcreenOFF.png"

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if (ExGlobal.lockscreenOpen)
                        ExGlobal.lockscreenOpen = false
                    else
                        ExGlobal.lockscreenOpen = true
                }
            }
        }

        Text {
            id: labelscreen
            text: qsTr("锁屏时间")
            x:49
            y:160
            font.pixelSize: 40
            color: ExGlobal.lockscreenOpen?"#000000":"#7f7f7f"
        }

        ComboBox {
            id :lockcombo
            anchors.right: lockscreenswitch.right
            anchors.rightMargin: 0
            anchors.verticalCenter: labelscreen.verticalCenter
            width: 293
            height: 60
            enabled: ExGlobal.lockscreenOpen?true:false
            currentIndex: ExGlobal.lockTime
            model:["1min","3min","5min","10min","15min","30min"]
            onCurrentIndexChanged: {
                if (ExGlobal.lockTime != currentIndex)
                    ExGlobal.lockTime = currentIndex
            }
        }
    }

    Bt1 {
        id: btCannel
        onClicked: {
            Sequence.setTitle("setup")
            mainView.pop();
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("setup_lockscreen")
    }
}
