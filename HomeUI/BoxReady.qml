import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    id: boxready_page

    Image {
        id: startbutton
        width: 200
        height: 200
        anchors.centerIn: parent

        source: "qrc:/image/start.png"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                //Sequence.sequenceDo(Sequence.Sequence_Test);
                if (ExGlobal.diskCheck() === 1){
                    testcancelDialog.show(2)
                }
                else {
                    mainView.pop();
                    if (ExGlobal.projectMode() === 0)
                        mainView.push("qrc:/HomeUI/TestProcessT.qml");
                    else
                        mainView.push("qrc:/HomeUI/TestProcess.qml");
                }
            }
        }
    }
    Text{
        id: panelName

        text:ExGlobal.panelName
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: -300
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: -30
    }
    Text{
        id: panelLot

        text:ExGlobal.boxSerial()
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: -300
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: +30
    }

    Rectangle{
        width: 400
        height: 200
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: +300
        anchors.verticalCenter: startbutton.verticalCenter
        //border.color: "black"
        Text{
            id: sampleNo
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.left: parent.left
            anchors.leftMargin: 100
            text:ExGlobal.sampleCode
            font.pixelSize: 30
        }
        Text{
            id: sampleInfo

            text:ExGlobal.sampleInfo
            font.pixelSize: 30
            anchors.horizontalCenter: sampleNo.horizontalCenter
            anchors.top: sampleNo.bottom
            anchors.topMargin: 20
        }
        Image {
            id: editsample
            anchors.left: sampleNo.right
            anchors.leftMargin: 70
            anchors.top: sampleNo.top
            anchors.topMargin: 30
            scale: 2
            source: "qrc:/image/editInfo.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                queryDlg.show(sampleInfo.text)
            }
        }
    }

    TwoQuery{
        id: queryDlg
        qtitle: qsTr("输入样本信息")
        qlable1: qsTr("样本号：")
        qlable2: qsTr("样本信息：")
        qtext1: sampleNo.text
        qtext2: sampleInfo.text
        anchors.fill: parent
        onQueryAck: {
            console.log(res1,res2);
            ExGlobal.sampleCode = res1;
            ExGlobal.sampleInfo = res2;
        }
    }

    Text{
        id: userName

        text:ExGlobal.user
        font.pixelSize: 30
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
    }

    Button {
        id: btCannel
        font.pixelSize: 30
        width: 200
        text: qsTr("取消测试")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            //mainView.pop();
            //mainView.push("qrc:/HomeUI/Idle.qml");
            //mainView.push("qrc:/HomeUI/TestCancel.qml");
            testcancelDialog.show(0)
        }
    }

    TestExitConfirm{
        id: testcancelDialog        
        anchors.fill: parent
        onQueryAck: {
            if (res === 0){
                mainView.pop();
                mainView.push("qrc:/HomeUI/Idle.qml");
            }
            else if(res === 2){
                mainView.pop();
                if (ExGlobal.projectMode() === 0)
                    mainView.push("qrc:/HomeUI/TestProcessT.qml");
                else
                    mainView.push("qrc:/HomeUI/TestProcess.qml");
            }
        }
    }

    Component.onCompleted: {
        headerMsg.text=qsTr("试剂盒就绪");
    }

    Connections{
        target: Sequence
        onPanelNameChanged:{
            panelName.text = ExGlobal.panelName;
            //panelLot.text = ExGlobal.panelCode;
            //panelLot.text = "Lot# " + ExGlobal.getCaliParam("PanelBoxIndex");
        }
    }

}
