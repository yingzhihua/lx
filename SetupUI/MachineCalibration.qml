import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
import "../components"
Rectangle {
    property int predoorpos: 0
    GroupBox{
        id:stepgroup
        title: qsTr("调整步长")
        anchors.right: parent.right
        anchors.rightMargin: 300
        y:50
        width: 600
        font.pixelSize: 40

        Row{
            spacing: 10
            RadioButton{
                id:rb1
                text:qsTr("1")
                onClicked: {
                    dev1Sp.stepSize=1
                    dev2Sp.stepSize=1
                    dev3Sp.stepSize=1
                    dev4Sp.stepSize=1
                    dev5Sp.stepSize=1
                    dev6Sp.stepSize=1
                }
            }
            RadioButton{
                id:rb2
                text:qsTr("10")
                onClicked: {
                    dev1Sp.stepSize=10
                    dev2Sp.stepSize=10
                    dev3Sp.stepSize=10
                    dev4Sp.stepSize=10
                    dev5Sp.stepSize=10
                    dev6Sp.stepSize=10
                }
            }
            RadioButton{
                id:rb3
                checked: true
                text: qsTr("100")
                onClicked: {
                    dev1Sp.stepSize=100
                    dev2Sp.stepSize=100
                    dev3Sp.stepSize=100
                    dev4Sp.stepSize=100
                    dev5Sp.stepSize=100
                    dev6Sp.stepSize=100
                }
            }
            RadioButton{
                id:rb4
                text: qsTr("500")
                onClicked: {
                    dev1Sp.stepSize=500
                    dev2Sp.stepSize=500
                    dev3Sp.stepSize=500
                    dev4Sp.stepSize=500
                    dev5Sp.stepSize=500
                    dev6Sp.stepSize=500
                }
            }
            RadioButton{
                id:rb5
                text: qsTr("1000")
                onClicked: {
                    dev1Sp.stepSize=1000
                    dev2Sp.stepSize=1000
                    dev3Sp.stepSize=1000
                    dev4Sp.stepSize=1000
                    dev5Sp.stepSize=1000
                    dev6Sp.stepSize=1000
                }
            }
        }
    }

    Grid{
        id: gridDev
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: stepgroup.bottom
        anchors.topMargin: 50
        columns:8
        rowSpacing: 20
        columnSpacing: 30


        Text{
            id:dev1
            font.pixelSize: 40
            text:qsTr("阀一电机：")
        }
        Button{
            id:dev1bt
            font.pixelSize: 40
            text:qsTr("硬件复位")
            onClicked: {
                dev1Sp.value = 0;
                if (Sequence.actionDo("V1",1,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev1btS
            font.pixelSize: 40
            text:qsTr("软件复位")
            onClicked: {
                dev1Sp.value = ExGlobal.getCaliParam("V1SoftHomeX");
                if (Sequence.actionDo("V1",2,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev1btT
            font.pixelSize: 40
            text:qsTr("工装位")
            onClicked: {
                dev1Sp.value = ExGlobal.getCaliParam("V1ToolHomeX");
                if (Sequence.actionDo("V1",9,dev1Sp.value,0,0))
                //if (Sequence.actionDo("V1",3,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Text{
            id:dev1Text
            font.pixelSize: 40
            text:ExGlobal.getCaliParam("V1ToolHomeX")
        }
        SpinBox{
            id:dev1Sp
            from:0
            value: 0
            to:100000
            stepSize: 100
            font.pixelSize: 40
            width: 220
        }
        Button{
            id:dev1Set
            font.pixelSize: 40
            text:qsTr("设置")
            onClicked: {
                if (Sequence.actionDo("V1",9,dev1Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev1Save
            font.pixelSize: 40
            text:qsTr("保存")
            onClicked: {
                ExGlobal.updateCaliParam("V1ToolHomeX",dev1Sp.value);
                ExGlobal.updateCaliParam("V1WorkX",dev1Sp.value-ExGlobal.getCaliParam("V1SoftHomeOffset")+ExGlobal.getCaliParam("V1WorkOffset"));
                ExGlobal.updateCaliParam("V1SoftHomeX",dev1Sp.value-ExGlobal.getCaliParam("V1SoftHomeOffset"));
                dev1Text.text = ExGlobal.getCaliParam("V1ToolHomeX");
            }
        }
        Text{
            id:dev2
            font.pixelSize: 40
            text:qsTr("阀二电机：")
        }
        Button{
            id:dev2bt
            font.pixelSize: 40
            text:qsTr("硬件复位")
            onClicked: {
                dev2Sp.value = 0;
                if (Sequence.actionDo("V2",1,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev2btS
            font.pixelSize: 40
            text:qsTr("软件复位")
            onClicked: {
                dev2Sp.value = ExGlobal.getCaliParam("V2SoftHomeX");
                if (Sequence.actionDo("V2",2,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev2btT
            font.pixelSize: 40
            text:qsTr("工装位")
            onClicked: {
                dev2Sp.value = ExGlobal.getCaliParam("V2ToolHomeX");
                if (Sequence.actionDo("V2",9,dev2Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Text{
            id:dev2Text
            font.pixelSize: 40
            text:ExGlobal.getCaliParam("V2ToolHomeX")
        }
        SpinBox{
            id:dev2Sp
            from:0
            value: 0
            to:100000
            stepSize: 100
            font.pixelSize: 40
            width: 220
        }
        Button{
            id:dev2Set
            font.pixelSize: 40
            text:qsTr("设置")
            onClicked: {
                if (Sequence.actionDo("V2",9,dev2Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev2Save
            font.pixelSize: 40
            text:qsTr("保存")
            onClicked: {
                ExGlobal.updateCaliParam("V2ToolHomeX",dev2Sp.value);
                ExGlobal.updateCaliParam("V2WorkX",dev2Sp.value-ExGlobal.getCaliParam("V2SoftHomeOffset")+ExGlobal.getCaliParam("V2WorkOffset"));
                ExGlobal.updateCaliParam("V2SoftHomeX",dev2Sp.value-ExGlobal.getCaliParam("V2SoftHomeOffset"));
                dev2Text.text = ExGlobal.getCaliParam("V2ToolHomeX");
                console.log("dev2Save,"+dev2Sp.value+",V2SoftHomeOffset:"+ExGlobal.getCaliParam("V2SoftHomeOffset")+"V2WorkOffset:"+ExGlobal.getCaliParam("V2WorkOffset"));
            }
        }

        Text{
            id:dev3
            font.pixelSize: 40
            text: qsTr("阀三电机：")
        }
        Button{
            id:dev3bt
            font.pixelSize: 40
            text:qsTr("硬件复位")
            onClicked: {
                dev3Sp.value = 0;
                if (Sequence.actionDo("V3",1,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev3btS
            font.pixelSize: 40
            text:qsTr("软件复位")
            onClicked: {
                dev3Sp.value = ExGlobal.getCaliParam("V3SoftHomeX");
                if (Sequence.actionDo("V3",2,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev3btT
            font.pixelSize: 40
            text:qsTr("工装位")
            onClicked: {
                dev3Sp.value = ExGlobal.getCaliParam("V3ToolHomeX");
                if (Sequence.actionDo("V3",9,dev3Sp.value,0,0))
                //if (Sequence.actionDo("V3",3,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Text{
            id:dev3Text
            font.pixelSize: 40
            text:ExGlobal.getCaliParam("V3ToolHomeX")
        }
        SpinBox{
            id:dev3Sp
            from:0
            value: 0
            to:100000
            stepSize: 100
            font.pixelSize: 40
            width: 220
        }
        Button{
            id:dev3Set
            font.pixelSize: 40
            text:qsTr("设置")
            onClicked: {
                if (Sequence.actionDo("V3",9,dev3Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev3Save
            font.pixelSize: 40
            text:qsTr("保存")
            onClicked: {
                ExGlobal.updateCaliParam("V3ToolHomeX",dev3Sp.value);
                ExGlobal.updateCaliParam("V3WorkX",dev3Sp.value-ExGlobal.getCaliParam("V3SoftHomeOffset")+ExGlobal.getCaliParam("V3WorkOffset"));
                ExGlobal.updateCaliParam("V3SoftHomeX",dev3Sp.value-ExGlobal.getCaliParam("V3SoftHomeOffset"));
                dev3Text.text = ExGlobal.getCaliParam("V3ToolHomeX");
            }
        }

        Text{
            id:dev4
            font.pixelSize: 40
            text: qsTr("刺破电机：")
        }
        Button{
            id:dev4bt
            font.pixelSize: 40
            text:qsTr("硬件复位")
            onClicked: {
                dev4Sp.value = 0;
                if (Sequence.actionDo("VP",1,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev4btS
            font.pixelSize: 40
            text:qsTr("软件复位")
            onClicked: {
                dev4Sp.value = ExGlobal.getCaliParam("VPSoftHomeX");
                if (Sequence.actionDo("VP",2,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev4btT
            font.pixelSize: 40
            text:qsTr("工装位")
            onClicked: {
                dev4Sp.value = ExGlobal.getCaliParam("VPToolHomeX");
                if (Sequence.actionDo("VP",9,dev4Sp.value,0,0))
                //if (Sequence.actionDo("VP",3,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Text{
            id:dev4Text
            font.pixelSize: 40
            text:ExGlobal.getCaliParam("VPToolHomeX")
        }
        SpinBox{
            id:dev4Sp
            from:0
            value: 0
            to:100000
            stepSize: 100
            font.pixelSize: 40
            width: 220
        }
        Button{
            id:dev4Set
            font.pixelSize: 40
            text:qsTr("设置")
            onClicked: {
                if (Sequence.actionDo("VP",9,dev4Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev4Save
            font.pixelSize: 40
            text:qsTr("保存")
            onClicked: {
                ExGlobal.updateCaliParam("VPToolHomeX",dev4Sp.value);
                ExGlobal.updateCaliParam("VPWorkX",dev4Sp.value-ExGlobal.getCaliParam("VPSoftHomeOffset")+ExGlobal.getCaliParam("VPWorkOffset"));
                ExGlobal.updateCaliParam("VPSoftHomeX",dev4Sp.value-ExGlobal.getCaliParam("VPSoftHomeOffset"));
                dev4Text.text = ExGlobal.getCaliParam("VPToolHomeX");
            }
        }

        Text{
            id:dev5
            font.pixelSize: 40
            text: qsTr("舱门电机：")
        }
        Button{
            id:dev5bt
            font.pixelSize: 40
            text:qsTr("硬件复位")
            onClicked: {
                dev5Sp.value = 0;
                predoorpos = dev5Sp.value;
                if (Sequence.actionDo("Door",5,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev5btS
            font.pixelSize: 40
            text:qsTr("出仓位置")
            onClicked: {
                dev5Sp.value = 0;
                predoorpos = dev5Sp.value;
                if (Sequence.actionDo("Door",1,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev5btT
            font.pixelSize: 40
            text:qsTr("工作位")
            onClicked: {
                dev5Sp.value = ExGlobal.getCaliParam("VDWorkX");
                predoorpos = dev5Sp.value;
                if (Sequence.actionDo("Door",2,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Text{
            id:dev5Text
            font.pixelSize: 40
            text:ExGlobal.getCaliParam("VDWorkX")
        }
        SpinBox{
            id:dev5Sp
            from:0
            value: 0
            to:100000
            stepSize: 100
            font.pixelSize: 40
            width: 220
        }
        Button{
            id:dev5Set
            font.pixelSize: 40
            text:qsTr("设置")
            onClicked: {
                if (predoorpos < dev5Sp.value)
                {
                    if (Sequence.actionDo("Door",4,dev5Sp.value-predoorpos,0,0))
                        busyIndicator.running = true;
                }
                else
                {
                    if (Sequence.actionDo("Door",3,predoorpos-dev5Sp.value,0,0))
                        busyIndicator.running = true;
                }
                predoorpos = dev5Sp.value;
            }
        }
        Button{
            id:dev5Save
            font.pixelSize: 40
            text:qsTr("保存")
            onClicked: {
                ExGlobal.updateCaliParam("VDWorkX",dev5Sp.value);
                dev5Text.text = ExGlobal.getCaliParam("VDWorkX");
            }
        }

        //pump
        Text{
            id:dev6
            font.pixelSize: 40
            text: qsTr("注射电机：")
        }
        Button{
            id:dev6bt
            font.pixelSize: 40
            text:qsTr("硬件复位")
            onClicked: {
                dev6Sp.value = 0;
                if (Sequence.actionDo("Pump",1,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev6btS
            font.pixelSize: 40
            text:qsTr("软件复位")
            onClicked: {
                dev6Sp.value = ExGlobal.getCaliParam("PumpSoftHomeX");
                if (Sequence.actionDo("Pump",9,dev6Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev6btT
            font.pixelSize: 40
            text:qsTr("工装位")
            onClicked: {
                dev6Sp.value = ExGlobal.getCaliParam("PumpToolHomeX");
                if (Sequence.actionDo("Pump",9,dev6Sp.value,0,0))
                //if (Sequence.actionDo("VP",3,0,0,0))
                    busyIndicator.running = true;
            }
        }
        Text{
            id:dev6Text
            font.pixelSize: 40
            text:ExGlobal.getCaliParam("PumpToolHomeX")
        }
        SpinBox{
            id:dev6Sp
            from:-30000
            value: 0
            to:30000
            stepSize: 100
            font.pixelSize: 40
            width: 220
        }
        Button{
            id:dev6Set
            font.pixelSize: 40
            text:qsTr("设置")
            onClicked: {
                if (Sequence.actionDo("Pump",9,dev6Sp.value,0,0))
                    busyIndicator.running = true;
            }
        }
        Button{
            id:dev6Save
            font.pixelSize: 40
            text:qsTr("保存")
            onClicked: {
                ExGlobal.updateCaliParam("PumpToolHomeX",dev6Sp.value);
                ExGlobal.updateCaliParam("PumpSoftHomeX",dev6Sp.value-ExGlobal.getCaliParam("PumpSoftHomeOffset"));
                dev6Text.text = ExGlobal.getCaliParam("PumpToolHomeX");
            }
        }

    }

    Button{
        id:btfan1
        anchors.left: gridDev.left
        anchors.leftMargin: 0
        anchors.top: gridDev.bottom
        anchors.topMargin: 30
        font.pixelSize: 40
        text:qsTr("获取转速")
        onClicked: {
            if (Sequence.actionDo("Fan",3,1,0,0))
                busyIndicator.running = true;
        }
    }

    Text {
        id: txfun1
        anchors.left: btfan1.right
        anchors.leftMargin: 50
        anchors.verticalCenter: btfan1.verticalCenter
        font.pixelSize: 40
        text: qsTr("风扇一转速：")+Sequence.fan1Speed
    }

    Button {
        id: btSave
        height: 60
        width: 200
        anchors.top: gridDev.bottom
        anchors.topMargin: 60
        anchors.horizontalCenter: gridDev.horizontalCenter
        font.pixelSize: 40
        highlighted: true
        visible: false
        text: qsTr("保存")

        onClicked: {
            ExGlobal.updateCaliParam("V1ToolHomeX",dev1Sp.value);
            ExGlobal.updateCaliParam("V1WorkX",dev1Sp.value-ExGlobal.getCaliParam("V1SoftHomeOffset")+ExGlobal.getCaliParam("V1WorkOffset"));
            ExGlobal.updateCaliParam("V1SoftHomeX",dev1Sp.value-ExGlobal.getCaliParam("V1SoftHomeOffset"));
            ExGlobal.updateCaliParam("V2ToolHomeX",dev2Sp.value);
            ExGlobal.updateCaliParam("V2WorkX",dev2Sp.value-ExGlobal.getCaliParam("V2SoftHomeOffset")+ExGlobal.getCaliParam("V2WorkOffset"));
            ExGlobal.updateCaliParam("V2SoftHomeX",dev2Sp.value-ExGlobal.getCaliParam("V2SoftHomeOffset"));
            ExGlobal.updateCaliParam("V3ToolHomeX",dev3Sp.value);
            ExGlobal.updateCaliParam("V3WorkX",dev3Sp.value-ExGlobal.getCaliParam("V3SoftHomeOffset")+ExGlobal.getCaliParam("V3WorkOffset"));
            ExGlobal.updateCaliParam("V3SoftHomeX",dev3Sp.value-ExGlobal.getCaliParam("V3SoftHomeOffset"));
            ExGlobal.updateCaliParam("VPToolHomeX",dev4Sp.value);
            ExGlobal.updateCaliParam("VPWorkX",dev4Sp.value-ExGlobal.getCaliParam("VPSoftHomeOffset")+ExGlobal.getCaliParam("VPWorkOffset"));
            ExGlobal.updateCaliParam("VPSoftHomeX",dev4Sp.value-ExGlobal.getCaliParam("VPSoftHomeOffset"));
            ExGlobal.updateCaliParam("VDWorkX",dev5Sp.value);

            dev1Text.text = ExGlobal.getCaliParam("V1ToolHomeX");
            dev2Text.text = ExGlobal.getCaliParam("V2ToolHomeX");
            dev3Text.text = ExGlobal.getCaliParam("V3ToolHomeX");
            dev4Text.text = ExGlobal.getCaliParam("VPToolHomeX");
            dev5Text.text = ExGlobal.getCaliParam("VDWorkX");
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
            Sequence.changeTitle(qsTr("设置"));
        }
    }

    BusyIndicator{
        id:busyIndicator
        anchors.centerIn: parent
        implicitWidth: 200
        implicitHeight: 200
        opacity: 1.0
        running: false
    }

    MouseArea{
        anchors.fill: parent
        enabled: busyIndicator.running?true:false
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            if (result == Sequence.Result_Simple_ok)
            {
                busyIndicator.running = false;
            }
        }
    }
}
