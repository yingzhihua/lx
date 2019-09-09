import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    property alias errText: dep.text
    anchors.fill: parent
    Text {
        id: dep
        anchors.centerIn: parent
        font.pixelSize: 30
        text: qsTr("text")
    }
}
