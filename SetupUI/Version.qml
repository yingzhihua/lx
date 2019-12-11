import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import "../components"
Item {
    Text {
        id: ver
        font.pixelSize: 40
        y:200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -200
        text: qsTr("主机板版本：")+ExGlobal.version
    }
    Text {
        id: tempver
        anchors.top: ver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("温控板版本：")+ExGlobal.tempversion
    }
    Text {
        id: ctrlver
        anchors.top: tempver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("驱动板版本：")+ExGlobal.ctrlversion
    }
    Text {
        id: ipver
        anchors.top: ctrlver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("本机IP地址：")+ExGlobal.getIP()
    }
    Text {
        id: updatelog
        anchors.left: ver.right
        anchors.leftMargin: 200
        y: 20
        wrapMode: Text.Wrap
        text:"V2.08\n1、增加测试用时显示。\nV2.09\n1、关闭背光补偿，背光补偿值：0，1，2，缺省值：1"
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
}
