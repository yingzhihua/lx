import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Page {
    property string titlemsg: "设置"
    id: setup_page

    StackView{
        id: setupView
        anchors.fill: parent
        initialItem: "./SetupUI/SetupMenu.qml"
    }
    onTitlemsgChanged: {
        testCompleted.width = 0;
        testunCompleted.width = 0;
        headerMsg.text = titlemsg;
    }
}

