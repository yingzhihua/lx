import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0

Page {
    id: testcancel_page
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
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 50
            text: qsTr("返回测试")
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
    }
}
