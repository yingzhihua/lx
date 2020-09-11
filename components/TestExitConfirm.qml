import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0

Rectangle {
    id: testcancelDialog
    property int dialogstatus: 0    
    signal queryAck(int res)
    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)

    Rectangle{
        z:2
        width: 811
        height: 485
        anchors.centerIn: parent
        //border.color: "darkgray"
        radius: 22

        Image {
            anchors.fill: parent
            source: "qrc:/images/loginbk.png"
        }

        Text{
            id:cancelText
            y:162
            width: 700
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 50
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAnywhere
            color: "#323232"
        }

        Bt2{
            id: continueTest
            x:43
            y:355
            width: 280
            height: 85
            textoffsetx: 30
            textoffsety: -3
            textcolor: "#464646"
            text: qsTr("返回测试")
            image: "qrc:/images/backTestbt.png"
            onClicked: {
                testcancelDialog.visible = false
                if (dialogstatus === 2)
                    queryAck(2)
                else
                    queryAck(1)
            }
        }

        Bt2{
            id: btConfirm
            x:489
            y:355
            width: 280
            height: 85
            textoffsetx: 30
            textoffsety: -3
            textcolor: "#ffffff"
            text: qsTr("确认取消")
            image: "qrc:/images/cancelTestbt.png"
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
            cancelText.text = qsTr("是否取消测试？")
            continueTest.text = qsTr("返回测试")
        }
        else if (status === 1)
        {
            cancelText.text = qsTr("测试进行中，取消后试剂盒作废 是否取消测试？")
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
