import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

Page {
    id: boxready_page
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
        anchors.horizontalCenterOffset: -300
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: -30
    }
    Text{
        id: panelLot

        text:ExGlobal.panelCode
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: -300
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: +30
    }
    Text{
        id: sampleNo

        text:ExGlobal.sampleCode
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: +300
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: -30
    }
    Text{
        id: sampleInfo

        text:ExGlobal.sampleInfo
        font.pixelSize: 30
        anchors.horizontalCenter: startbutton.horizontalCenter
        anchors.horizontalCenterOffset: +300
        anchors.verticalCenter: startbutton.verticalCenter
        anchors.verticalCenterOffset: +30
    }    

    Text{
        id: message
        height: 100
        width: parent.width - 40
        text:"ExGlobal.s"
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: sampleInfo.bottom
        anchors.topMargin: 40
        wrapMode: Text.Wrap
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
            Sequence.sequenceCancel();            
        }
    }

    Component.onCompleted: {
        Sequence.sequenceDo(Sequence.Sequence_Test);
    }

    Connections{
        target: Sequence
        onProcessFinish:{
            console.log("TestProcess.qml,total:"+total+",finish:"+finish);
            var ffinish = finish*1000/total;
            //if (ffinish == 0) ffinish = 1;
            home_page.testprocess = ffinish;
            if (total == finish)
            {
                headerMsg.text = "测试完成！";
                stackView.pop();
                stackView.push("qrc:/HomeUI/Idle.qml");
            }
            else
                headerMsg.text = "预计剩余"+((total-finish)/1000)+"秒";
            message.text = Sequence.sequenceMessage();
        }

        onSequenceFinish:{
            if (result == Sequence.Result_SelfCheck_ok)
            {
                stackView.pop();
                stackView.push("qrc:/HomeUI/Login.qml");
            }
            else if(result == Sequence.Result_CannelTest_ok)
            {
                stackView.pop();
                stackView.push("qrc:/HomeUI/Idle.qml");
            }

            console.log("TestProcess.qml,result:"+result);
        }
    }
}
