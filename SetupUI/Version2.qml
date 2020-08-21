import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
import "../components"
Item {
    Text{
        id: verLabel
        y:300
        x:600
        font.pixelSize: 35
        text: qsTr("软件发布版本号：")
    }
    Text{
        id: ver
        y:300
        anchors.right: parent.right
        anchors.rightMargin: verLabel.x
        font.pixelSize: 35
        color: "#898989"
        text: ExGlobal.version
    }
    Rectangle{
        x:verLabel.x
        y:verLabel.y+70
        width: parent.width - verLabel.x*2
        height: 2
        color: "#c8c8c8"
    }

    Text{
        id: ver2Label
        y:verLabel.y+100
        x:verLabel.x
        font.pixelSize: 35
        text: qsTr("软件完整版本号：")
    }
    Text{
        id: ver2
        y:verLabel.y+100
        anchors.right: parent.right
        anchors.rightMargin: verLabel.x
        font.pixelSize: 35
        color: "#898989"
        text: ExGlobal.buildversion
    }
    Rectangle{
        x:verLabel.x
        y:ver2Label.y+70
        width: parent.width - verLabel.x*2
        height: 2
        color: "#c8c8c8"
    }

    Text{
        id: ver3Label
        y:ver2Label.y+100
        x:verLabel.x
        font.pixelSize: 35
        text: qsTr("仪器规格型号：")
    }
    Text{
        id: ver3
        y:ver2Label.y+100
        anchors.right: parent.right
        anchors.rightMargin: verLabel.x
        font.pixelSize: 35
        color: "#898989"
        text: "FDx-500"
    }
    Rectangle{
        x:verLabel.x
        y:ver3Label.y+70
        width: parent.width - verLabel.x*2
        height: 2
        color: "#c8c8c8"
    }
/*
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
*/
    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.right: parent.right
        anchors.rightMargin: 120
        onClicked: {
            mainView.pop();
            Sequence.hideTitle(true);
        }
    }

    Component.onCompleted: Sequence.hideTitle(false);
}
