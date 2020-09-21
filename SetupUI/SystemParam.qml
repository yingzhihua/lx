import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
Item {
    GroupBox{
        id:softVersion
        title: qsTr("软件版本")
        anchors.horizontalCenter: parent.horizontalCenter
        y:100
        width: 300
        font.pixelSize: 40

        Column{
            spacing: 10
            RadioButton{
                id:rb1
                text:qsTr("正式软件")
                onClicked: {
                    ExGlobal.updateCaliParam("ProjectMode",1);
                }
            }
            RadioButton{
                id:rb2
                text:qsTr("工程软件")
                onClicked: {
                    ExGlobal.updateCaliParam("ProjectMode",0);
                }
            }
            RadioButton{
                id:rb3
                checked: true
                text: qsTr("送检软件")
                onClicked: {
                    ExGlobal.updateCaliParam("ProjectMode",2);
                }
            }
        }
    }

    Bt1 {
        id: btCannel
        onClicked: {
            mainView.pop();
        }
    }

    Button {
        id: btExit
        font.pixelSize: 30
        width: 200
        height: 70
        text: qsTr("退出")
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 60
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
           close()
        }
    }

    Component.onCompleted: {
        if (ExGlobal.getCaliParam("ProjectMode")===1)
            rb1.checked = true
        else if (ExGlobal.getCaliParam("ProjectMode")===0)
            rb2.checked = true;
        else if (ExGlobal.getCaliParam("ProjectMode")===2)
            rb3.checked = true;
    }
}
