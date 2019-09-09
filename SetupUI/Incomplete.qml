import QtQuick 2.0
import "../components"
Item {
    Text {
        id: ver
        font.pixelSize: 40
        anchors.centerIn: parent
        text: qsTr("待建中。。。")
    }
    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            setupView.pop();
        }
    }
}
