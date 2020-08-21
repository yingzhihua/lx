import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    id: boxready_page
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

        Image {
            id: startbutton
            width: 220
            height: 224
            x:850
            y:287
            fillMode: Image.Pad

            source: "qrc:/images/TestStart.png"
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
                onPressed: parent.x += 10
                onReleased: parent.x -= 10
            }
        }
        Text{
            font.pixelSize: 45
            font.bold: true
            anchors.top: startbutton.bottom
            anchors.topMargin: 28
            anchors.horizontalCenter: startbutton.horizontalCenter
            text: qsTr("开始测试")
        }

        ThreeMessage{
            x:358
            y:224
            text1: ExGlobal.panelName
            text3: ExGlobal.boxSerial()
        }

        ThreeMessage{
            x:1159
            y:224
            text1: ExGlobal.sampleCode
            text2: ExGlobal.sampleInfo
        }

        Image {
            id: editsample
            x: 1621
            y: 606
            height: 75
            width: 70
            fillMode: Image.Pad
            source: "qrc:/images/SampleInput.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    queryDlg.show()
                }
            }
        }

        ThreeQuery{
            id: queryDlg
            qtitle: qsTr("输入样本信息")
            qlable1: qsTr("样本号")
            qlable2: qsTr("样本信息")
            qlable3: qsTr("样本备注")
            qtext1: ExGlobal.sampleCode
            qtext2: ExGlobal.sampleInfo
            anchors.fill: parent
            onQueryAck: {
                console.log(res1,res2);
                ExGlobal.sampleCode = res1;
                ExGlobal.sampleInfo = res2;
            }
        }

        Image {
            id: btQuit
            x: 1450
            y: 754
            height: 106
            width: 299
            fillMode: Image.Pad
            source: "qrc:/images/TestCancel.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    testcancelDialog.show(0)
                }
            }
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
