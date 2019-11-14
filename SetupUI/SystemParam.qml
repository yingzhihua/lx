import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
Item {
    Grid{
        id: paramGrid
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100
        columns:2
        rowSpacing: 30
        columnSpacing: 30

        Text{
            width: 80
            height: 30
            text: qsTr("系统名称：")
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 40
        }

        TextField{
            id:inputName
            anchors.verticalCenter: labelName.verticalCenter
            anchors.left: labelName.right
            anchors.leftMargin: 50
            width:300
            height: 50
            verticalAlignment: Text.AlignBottom
            font.pixelSize: 20
            background: Rectangle{
                border.color: "darkgray"
                radius: 5
            }
        }

        Text{
            width: 80
            height: 30
            text: qsTr("锁屏时间：")
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 40
        }

        SpinBox{
            id:spinLock
            from:0
            value: 10
            to:100
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
            //*
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
            //*/
        }
    }

    Button{
        id: btSave
        anchors.top: paramGrid.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: paramGrid.horizontalCenter
        width: 150
        height: 50

        contentItem: Text {
            text: qsTr("设置")
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
        onClicked: {
            console.log(inputName.text);
        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            setupView.pop();
        }
    }

    Component.onCompleted: {
        inputName.text = ExGlobal.sysName
    }

}
