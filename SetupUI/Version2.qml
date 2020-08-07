import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
import "../components"
Item {
    Text {
        id: ver
        font.pixelSize: 60
        y:300
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -200
        text: qsTr("软件发布版本号：")+ExGlobal.version
    }
    Text {
        id: ver2
        anchors.top: ver.bottom
        anchors.topMargin: 40
        anchors.left: ver.left
        anchors.leftMargin: 0
        font.pixelSize: 60
        text: qsTr("软件完整版本号：")+ExGlobal.buildversion
    }
    Text {
        id: machineType
        anchors.top: ver2.bottom
        anchors.topMargin: 30
        anchors.left: ver.left
        anchors.leftMargin: 0
        font.pixelSize: 60
        text: qsTr("仪器规格型号：FDx-500")
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
}
