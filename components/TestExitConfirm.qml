import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0

Page {
    id: testcancelDialog
    property int dialogstatus: 0
    //property alias prompt: cancelText.text
    signal queryAck(int res)
    visible: false
    Text {
        id: cancelText
        font.pixelSize: 60
        anchors.horizontalCenter: parent.horizontalCenter
        y:200
        text: qsTr("取消测试后，试剂盒将不再可用！")
    }

    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -300
        anchors.top: cancelText.bottom
        anchors.topMargin: 200
        height: 150
        width: 450
        border.color: "darkgray"
        color: "darkgray"
        radius: 30

        Text {
            id: continueTest
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 50
            text: qsTr("返回测试")
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                testcancelDialog.visible = false
                if (dialogstatus === 2)
                    queryAck(2)
                else
                    queryAck(1)
            }
        }
    }

    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 300
        anchors.top: cancelText.bottom
        anchors.topMargin: 200
        height: 150
        width: 450
        border.color: "darkgray"
        color: "darkgray"
        radius: 30

        Text {
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 50
            text: qsTr("确认取消")
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                testcancelDialog.visible = false
                queryAck(0)
            }
        }
    }

    function show(status){
        //cancelText.text = prompt;
        dialogstatus = status;
        if (status === 0)
        {
            cancelText.text = qsTr("确定取消测试？")
            continueTest.text = qsTr("返回测试")
        }
        else if (status === 1)
        {
            cancelText.text = qsTr("取消测试后，试剂盒将不再可用，确定取消测试？")
            continueTest.text = qsTr("返回测试")
        }
        else if (status === 2)
        {
            cancelText.text = qsTr("磁盘空间剩余不多，是否继续测试？")
            continueTest.text = qsTr("继续测试")
        }
        else if (status === 3)
        {
            cancelText.text = qsTr("吸液异常，是否继续测试？")
            continueTest.text = qsTr("继续测试")
        }

        testcancelDialog.visible = true;
    }
}
