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
            width: 219
            height: 224
            x:850
            y:287
            fillMode: Image.Pad

            source: "qrc:/images/StartTest.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    //Sequence.sequenceDo(Sequence.Sequence_Test);
                    ExGlobal.setSampleType(sampleTypecombo.currentIndex)
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
            id:readyTips
            font.pixelSize: ExGlobal.uiParam[ExGlobal.UI_BOXREADY_BUTTOM_PROMPT_FONT]
            font.bold: true
            anchors.top: startbutton.bottom
            anchors.topMargin: 28
            anchors.horizontalCenter: startbutton.horizontalCenter
            color: "#4c4c4c"
            wrapMode: Text.WrapAnywhere
            width: 350
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("开始测试")
        }

        ThreeMessage{
            x:358
            y:224
            text1: ExGlobal.panelName+ExGlobal.panelCode
            text2: ExGlobal.boxSerial()
        }

        ComboBox {
            id :sampleTypecombo
            x:390
            y:570
            width: 300
            height: 60
            font.pixelSize: 40
            model: ExGlobal.getSampleTypeArr()
        }

        ThreeMessage{
            x:1195
            y:224
            text1: ExGlobal.sampleCode
            text2: ExGlobal.sampleInfo
            text3: ExGlobal.sampleRemark
        }

        Image {
            id: editsample
            x: 1621
            y: 606
            height: 75
            width: 70
            fillMode: Image.Pad
            source: "qrc:/images/Pen.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    queryDlg.show()
                }
            }
        }

        ThreeQuery{
            id: queryDlg
            qtitle: qsTr("样本信息录入")
            qlable1: qsTr("样本号")
            qlable2: qsTr("样本信息")
            qlable3: qsTr("样本备注")
            qtext1: ExGlobal.sampleCode
            qtext2: ExGlobal.sampleInfo
            qtext3: ExGlobal.sampleRemark
            anchors.fill: parent
            onQueryAck: {
                console.log(res1,res2,res3);
                ExGlobal.sampleCode = res1;
                ExGlobal.sampleInfo = res2;
                ExGlobal.sampleRemark = res3;
            }
        }

        Bt2{
            x:1450
            y:754
            height: 106
            width: 299
            textoffsetx: 20
            textoffsety: -4
            image: "qrc:/images/CancelTest.png"
            text: qsTr("取消测试")
            textcolor: "#323232"
            textfont: ExGlobal.uiParam[ExGlobal.UI_BOXREADY_TEST_CANCEL_FONT]
            onClicked: testcancelDialog.show(0)
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
        Sequence.setTitle("boxready");
        Sequence.uiStage = Sequence.Stage_ready;        
        if (ExGlobal.sampleCode === "")
        {
            startbutton.source = "qrc:/images/UStartTest.png"
            startbutton.enabled = false
            readyTips.text = qsTr("样本号不能为空 请输入样本号")
            readyTips.color = "#FC7052"
        }
        console.log("UI_BOXREADY_BUTTOM_PROMPT_FONT",ExGlobal.uiParam[ExGlobal.UI_BOXREADY_BUTTOM_PROMPT_FONT]);
    }

    Connections{
        target: Sequence
        onPanelNameChanged:{
            panelName.text = ExGlobal.panelName;
        }
    }

    Connections{
        target: ExGlobal
        onSampleCodeChanged:{
            if (ExGlobal.sampleCode !== "")
            {
                startbutton.source = "qrc:/images/StartTest.png"
                startbutton.enabled = true
                readyTips.text = qsTr("开始测试")
                readyTips.color = "#4c4c4c"
            }
            else{
                startbutton.source = "qrc:/images/UStartTest.png"
                startbutton.enabled = false
                readyTips.text = qsTr("样本号不能为空 请输入样本号")
                readyTips.color = "#FC7052"
            }
        }
    }

}
