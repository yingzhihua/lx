import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
Item {
    CheckBox{
        id:cbProjectMode
        anchors.horizontalCenter: parent.horizontalCenter
        y: 200
        text: "工程模式"
        onCheckedChanged: {
            ExGlobal.updateCaliParam("ProjectMode",checked?0:1);
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
        if (ExGlobal.getCaliParam("ProjectMode")===1)
            cbProjectMode.checked = false;
        else
            cbProjectMode.checked = true;

    }

}
