import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0

import "./components"

Page {
    property alias errText: dep.text
    anchors.fill: parent

    Bt2 {
        id: btClose
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -60
        height: 107
        width: 300
        textoffsetx: 50
        textoffsety: -4
        image: "qrc:/images/shutdown.png"
        text: qsTr("关机")
        onClicked: ExGlobal.exClose()
    }

    Text {
        id: dep
        anchors.top: btClose.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 45
        color: "#4c4c4c"
        horizontalAlignment:Text.AlignHCenter
    }

    Text{
        anchors.top: dep.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 45
        color: "#4c4c4c"
        horizontalAlignment: Text.AlignHCenter
        text:qsTr("请联系您的供应商")
    }
}
