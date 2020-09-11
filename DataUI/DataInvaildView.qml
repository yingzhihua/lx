import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    Rectangle{
        id:testmsg
        y:159
        width: parent.width
        height: 114
        color: "#DDDDDD"

        Text{
            id: panelName
            text:testModel.getCurrTestPanelName()
            font.pixelSize: 40
            anchors.left: parent.left
            anchors.leftMargin: 500
            anchors.verticalCenter: parent.verticalCenter
        }

        Text{
            id: panelCode
            text:testModel.getCurrTestPanelCode()
            font.pixelSize: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        Text{
            id: boxSerial
            text:testModel.getCurrTestSerial()
            font.pixelSize: 40
            anchors.right: parent.right
            anchors.rightMargin: 500
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Image {
        id: invalidico
        anchors.horizontalCenter: parent.horizontalCenter
        width: 61
        height: 55
        y:440
        source: "qrc:/images/thrAlarm.png"
    }
    Text{
        text:qsTr("测试无效\n内部参考品结果异常")
        font.pixelSize: 45
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: invalidico.bottom
        anchors.topMargin: 40
        horizontalAlignment:Text.AlignHCenter
    }

    Bt1 {
        id: btCannel
        onClicked: {
            mainView.pop();
            Sequence.setTitle("datamenu")
            if (ExGlobal.dataEntry() === 1)
                mainView.push("qrc:/HomeUI/Idle.qml");
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("dataview")
        Sequence.changeTitle(testModel.getCurrTestCode() + " | " + testModel.getCurrTestInfo())
    }

}
