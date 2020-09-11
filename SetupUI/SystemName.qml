import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
import Dx.Sequence 1.0
/*
Item {
    Text{
        id:labelName
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -100
        anchors.verticalCenterOffset: -50
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
    Button{
        id: btSave
        anchors.top: labelName.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: labelName.horizontalCenter
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
        onClicked: {            
            ExGlobal.sysName = inputName.text;
            setupView.pop();
        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            mainView.pop();
        }
    }

    Component.onCompleted: {
        inputName.text = ExGlobal.sysName
    }

}
*/
Page {
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

        Rectangle{
            id:updaterect
            width: 798
            height: 375
            radius: 22
            anchors.centerIn: parent

            Image {
                anchors.fill: parent
                source: "qrc:/images/upasswdbg.png"
            }

            Text{
                id:lhospital
                width: 170
                x:100
                y:86
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("医院名称")
            }

            TextField{
                id: hospital
                anchors.left: lhospital.right
                anchors.leftMargin: 30
                anchors.verticalCenter: lhospital.verticalCenter
                width: 400
                height: 64
                background: Rectangle{border.color: "#cdcdcd";radius: 5;color: "#f5f5f5"}
            }

            Text{
                id:ldevice
                anchors.left: lhospital.left
                anchors.leftMargin: 0
                width: lhospital.width
                anchors.top: lhospital.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("仪器名称")
            }

            TextField{
                id: device
                anchors.horizontalCenter: hospital.horizontalCenter
                anchors.verticalCenter: ldevice.verticalCenter
                width: 400
                height: 64
                background: Rectangle{border.color: "#cdcdcd";radius: 5;color: "#f5f5f5"}
            }

            Text{
                id:lprint
                anchors.left: lhospital.left
                anchors.leftMargin: 0
                width: lhospital.width
                anchors.top: ldevice.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("打印及设置")
            }

            ComboBox {
                id :cbprint
                anchors.horizontalCenter: hospital.horizontalCenter
                anchors.verticalCenter: lprint.verticalCenter
                width: 400
                height: 64
                model:[qsTr("便携式打印机"),qsTr("台式打印机")]
                background: Rectangle{border.color: "#cdcdcd";radius: 5;color: "#f5f5f5"}

            }
        }
        Text {
            id: mess
            anchors.top: updaterect.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: updaterect.horizontalCenter
        }
    }

    Bt2 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.right: parent.right
        anchors.rightMargin: 170
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        image: "qrc:/images/cancelbt.png"
        text:qsTr("取消")
        onClicked: {
            Sequence.setTitle("setup");
            mainView.pop();
        }
    }

    Bt2{
        id: btconfirm
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btCannel.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        textcolor: "#ffffff"
        image: "qrc:/images/confirmbt2.png"
        text:qsTr("确认")
        onClicked: {
            ExGlobal.hospitalName = hospital.text;
            ExGlobal.sysName = device.text;
            ExGlobal.printType = cbprint.currentIndex;
            Sequence.setTitle("setup");
            mainView.pop();
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("setup_system");
        hospital.text = ExGlobal.hospitalName;
        device.text = ExGlobal.sysName;
        cbprint.currentIndex = ExGlobal.printType;
    }
}
