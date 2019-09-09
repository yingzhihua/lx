import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3

import Dx.Sequence 1.0
import Dx.Global 1.0

Page {
    id: testProcessT

    Rectangle{
        id:mainAnaImage
        x:10
        y:10
        width:  1040//652
        height: 780//490
        color:"lightskyblue"

        Text{
            id:viewText
            font.pixelSize: 40
            anchors.centerIn: parent
        }
        Image {
            id: anaMainBox
            anchors.centerIn: parent
            width: parent.width - 4
            height: parent.height - 4
            source: ""
        }
    }
    Text{
        id: message
        height: 100
        width: parent.width - 40
        text:"ExGlobal.s"
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: mainAnaImage.bottom
        anchors.topMargin: 40
        wrapMode: Text.Wrap
    }

    ChartView {
        id: chartView
        title: "实时测试曲线"
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: mainAnaImage.right
        anchors.leftMargin: 1
        height: 700
        width: 900
        legend.alignment: Qt.AlignTop
        animationOptions: ChartView.SeriesAnimations
        antialiasing: true

        ValueAxis{
            id: axisX
            min: 0
            max: 50
            tickCount: 5
        }
        ValueAxis{
            id:axisY
            min:-0.5
            max:1.5
        }

        LineSeries{
            id:serial0
            name:"FLUA"

        }
        LineSeries{
            id:serial1
            name:"FLUB"
        }
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

            console.log("TestProcessT.qml,result:"+result);
        }

        onCallQmlRefeshAnaMainImg:{
            anaMainBox.source = "";
            anaMainBox.source = "image://CodeImg/AnaMain";
            console.log("CameraSetup.qml,onCallQmlRefeshAnaMainImg");
        }
    }
}
