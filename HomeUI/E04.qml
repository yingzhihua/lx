import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Sequence 1.0
import Dx.Global 1.0

Page {
    Text{
        id: errorDesp

        text:qsTr("试剂盒已过期")
        font.pixelSize: 60
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100
    }

    Text{

        text:qsTr("请取出试剂盒")
        font.pixelSize: 60
        anchors.top: errorDesp.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Connections{
        target: Sequence
        onBoxStateChanged:{
            if (!Sequence.box)
             {
                updateState.stop()
                mainView.pop();
                mainView.push("qrc:/HomeUI/Idle.qml");
            }
        }
    }

    Timer{
        id: updateState
        repeat: true
        interval: 500
        running: false
        onTriggered: {
            console.log("E04 query sensor state");
            Sequence.actionDo("Query",3,0,0,0);
        }
    }

    Component.onCompleted: {
        if (Sequence.boxParam() === 1)
            errorDesp.text = qsTr("试剂盒已使用");
        else if (Sequence.boxParam() === 3)
            errorDesp.text = qsTr("试剂盒不能识别");
        else if (Sequence.boxParam() === 4)
            errorDesp.text = qsTr("试剂盒未检测到二维码");
        else if (Sequence.boxParam() === 2)
            errorDesp.text = qsTr("空试剂盒");
        else if (Sequence.boxParam() === 5)
            errorDesp.text = qsTr("试剂盒已过期");
        updateState.start()
    }
}


