import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Page {
    property alias errText: dep.text
    anchors.fill: parent
    Text {
        id: dep
        anchors.centerIn: parent
        font.pixelSize: 30
        text: qsTr("text")
    }

    Button {
        id: btClose
        font.pixelSize: 30
        text: qsTr("关机")
        width: 200
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: ExGlobal.exClose()
    }
}
