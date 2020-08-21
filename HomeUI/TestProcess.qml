import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    id: test_page
    /*
    Image {
        id: startbutton
        anchors.centerIn: parent

        source: "qrc:/image/line.png"
    }
    Text{
        id: panelName

        text:ExGlobal.panelName
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: -400
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: -40
    }
    Text{
        id: panelLot

        text:ExGlobal.boxSerial()
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: -400
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: +40
    }
    Text{
        id: sampleNo

        text:ExGlobal.sampleCode
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: +400
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: -40
    }
    Text{
        id: sampleInfo

        text:ExGlobal.sampleInfo
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: +400
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: +40
    }    
*/
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

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
        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter
            y:240
            width: 2
            height: 400
            color: "#d9d9d9"
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
                    testcancelDialog.show(1)
                }
                onPressed: parent.x += 10
                onReleased: parent.x -= 10
            }
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
        Sequence.sequenceDo(Sequence.Sequence_Test);
    }

    Connections{
        target: Sequence
        onProcessFinish:{
            console.log("TestProcess.qml,total:"+total+",finish:"+finish);            
            message.text = Sequence.sequenceMessage();
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
                if (ExGlobal.projectMode() === 2){
                    headerMsg.text = "测试完成！";
                    closemsg.show(qsTr("测试完成，准备退出到待机！"))
                }
                else{
                    headerMsg.text = "测试完成！";
                    mainView.pop();
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

