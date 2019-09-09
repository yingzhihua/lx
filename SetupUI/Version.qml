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
        text: qsTr("主机板版本：")+ExGlobal.version
    }
    Text {
        id: tempver
        anchors.top: ver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 40
        text: qsTr("温控板版本：")+ExGlobal.tempversion
    }
    Text {
        id: ctrlver
        anchors.top: tempver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 40
        text: qsTr("驱动板版本：")+ExGlobal.ctrlversion
    }
    Text {
        id: ipver
        anchors.top: ctrlver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 40
        text: qsTr("本机IP地址：")+ExGlobal.getIP()
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
