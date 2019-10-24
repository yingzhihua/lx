import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    Text {
        id: msg
        text: qsTr("text")
    }
    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            dataView.pop();
        }
    }
    Component.onCompleted: {
        //msg.text = testModel.getCurrItem()
    }
}
