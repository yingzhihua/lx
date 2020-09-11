import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import Dx.Sequence 1.0
import Dx.Global 1.0
import "../components"
Rectangle {
    Grid{
        id: grid
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -200
        columns:2
        rowSpacing: 50
        columnSpacing: 30

        Label{
            text: qsTr("时序:")
        }
        ComboBox {
            id :panelcombo
            width: 400
            height: 60
            currentIndex: 0
            model: Sequence.LoopTestList;
        }
        Label{
            text:qsTr("循环数：")
        }
        SpinBox{
            id:testcount
            from:1
            value: 10
            to:2000
            stepSize: 1
            width: 400
        }
    }

    Grid{
        id: gridbt
        anchors.horizontalCenter: grid.horizontalCenter
        anchors.top: grid.bottom
        anchors.topMargin: 100
        columnSpacing: 50
        columns:2

        Button {
            id: btStart
            width: 200
            text: qsTr("开始测试")
            onClicked: {
                panelcombo.enabled = false
                testcount.enabled = false
                Sequence.loopTest(panelcombo.currentText,testcount.value)
            }
        }

        Button {
            id: btCancel
            width: 200
            text: qsTr("退出测试")
            onClicked: {
                Sequence.sequenceCancel();
            }
        }
    }

    Text{
        id: message
        height: 100
        width: parent.width - 40
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: gridbt.bottom
        anchors.topMargin: 40
        wrapMode: Text.Wrap
    }

    Bt1 {
        id: btCannel
        onClicked: {
            mainView.pop();
            Sequence.changeTitle(qsTr("设置"));
        }
    }

    Component.onCompleted: {
        Sequence.changeTitle(qsTr("耐久性测试"))
    }

    Connections{
        target: Sequence
        onProcessFinish:{
            message.text = Sequence.sequenceMessage();
        }
        onSequenceFinish:{
            if(result == Sequence.Result_LoopTest_finish){
                Sequence.changeTitle("耐久性测试完成！");
                panelcombo.enabled = true;
                testcount.enabled = true;
            }
            else if(result == Sequence.Result_CannelTest_ok)
            {
                Sequence.changeTitle("耐久性测试停止！");
                panelcombo.enabled = true;
                testcount.enabled = true;
            }
        }
    }

}
