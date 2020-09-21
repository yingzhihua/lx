import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
Item {
    GroupBox{
        id:clear
        title: qsTr("调焦清晰度判断")
        y:100
        x:100
        width: 340
        font.pixelSize: 40

        Column{
            spacing: 10
            RadioButton{
                id:c1
                text:qsTr("清晰度一")
                onClicked: {
                    ExGlobal.updateCaliParam("ClearMode",0);
                }
            }
            RadioButton{
                id:c2
                text:qsTr("清晰度二")
                onClicked: {
                    ExGlobal.updateCaliParam("ClearMode",1);
                }
            }
            RadioButton{
                id:c3
                text: qsTr("清晰度三")
                onClicked: {
                    ExGlobal.updateCaliParam("ClearMode",2);
                }
            }
            RadioButton{
                id:c4
                text:qsTr("亮度")
                onClicked: {
                    ExGlobal.updateCaliParam("ClearMode",3);
                }
            }
            RadioButton{
                id:c5
                text: qsTr("圆面积")
                onClicked: {
                    ExGlobal.updateCaliParam("ClearMode",4);
                }
            }
        }
    }

    GroupBox{
        id:machineType
        title: qsTr("仪器规格型号")
        anchors.left: clear.right
        anchors.leftMargin: 100
        anchors.top: clear.top
        width: clear.width
        font.pixelSize: 40

        Column{
            spacing: 10
            RadioButton{
                id:m1
                text:"FDx-500"
                onClicked: {
                    ExGlobal.updateCaliParam("MachineMode",0);
                }
            }
            RadioButton{
                id:m2
                text:"FDx-1000"
                onClicked: {
                    ExGlobal.updateCaliParam("MachineMode",1);
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

    Component.onCompleted: {
        if (ExGlobal.getCaliParam("ClearMode")===0)
            c1.checked = true
        else if (ExGlobal.getCaliParam("ClearMode")===1)
            c2.checked = true;
        else if (ExGlobal.getCaliParam("ClearMode")===2)
            c3.checked = true;
        else if (ExGlobal.getCaliParam("ClearMode")===3)
            c4.checked = true;
        else if (ExGlobal.getCaliParam("ClearMode")===4)
            c5.checked = true;

        if (ExGlobal.getCaliParam("MachineMode")===0)
            m1.checked = true
        else if (ExGlobal.getCaliParam("MachineMode")===1)
            m2.checked = true;
    }
}
