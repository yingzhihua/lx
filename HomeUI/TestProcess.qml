import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    id: test_page
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

        ThreeMessage{
            x:358
            y:224
            text1: ExGlobal.panelName+ExGlobal.panelCode
            text2: ExGlobal.boxSerial()
            text3: ExGlobal.getCurrSampleName()
        }

        ThreeMessage{
            x:1159
            y:224
            text1: ExGlobal.sampleCode
            text2: ExGlobal.sampleInfo
            text3: ExGlobal.sampleRemark
        }

        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter
            y:240
            width: 2
            height: 400
            color: "#d9d9d9"
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
            onClicked: testcancelDialog.show(1)
        }
    }

    TestExitConfirm{
        id: testcancelDialog        
        anchors.fill: parent
        onQueryAck: {
            if (res === 0){
                Sequence.sequenceCancel();
            }
        }
    }

    AutoCloseMsg{
        id:closemsg
        anchors.fill: parent
        onCloseAck: {
            mainView.pop();
            mainView.push("qrc:/HomeUI/Idle.qml");
        }
    }

    Component.onCompleted: {
        Sequence.uiStage = Sequence.Stage_test;
        Sequence.setTitle("test");
        Sequence.sequenceDo(Sequence.Sequence_Test);
    }

    Connections{
        target: Sequence
        onProcessFinish:{
            console.log("TestProcess.qml,total:"+total+",finish:"+finish);            
            //message.text = Sequence.sequenceMessage();
        }

        onSequenceFinish:{
            if (result == Sequence.Result_SelfCheck_ok)
            {
                mainView.pop();
                mainView.push("qrc:/HomeUI/Login.qml");
            }
            else if(result == Sequence.Result_CannelTest_ok)
            {
                mainView.pop();
                mainView.push("qrc:/HomeUI/Idle.qml");
            }
            else if(result == Sequence.Result_Test_finish){
                ExGlobal.sampleCode = "";
                ExGlobal.sampleInfo = "";
                ExGlobal.sampleRemark = "";

                if (ExGlobal.projectMode() === 2){
                    headerMsg.text = "测试完成！";
                    closemsg.show(qsTr("测试完成，准备退出到待机！"))
                }
                else{
                    headerMsg.text = "测试完成！";
                    mainView.pop();
                    if (ExGlobal.panelCode.substring(0,1) === "3")
                        mainView.push("qrc:/DataUI/OneDataLine.qml")
                    else
                        mainView.push("qrc:/DataUI/DataView.qml");
                    ExGlobal.setDataEntry(1);
                }
            }
            else if(result == Sequence.Result_Test_unfinish){
                headerMsg.text = "测试完成！";
                closemsg.show(qsTr("试剂盒异常，测试非正常完成！"))

            }
            else if(result == Sequence.Result_Test_DryFillErr){
                testcancelDialog.show(3)
            }

            console.log("TestProcess.qml,result:"+result);
        }
    }
}

