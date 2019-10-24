import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Page {
    property string titlemsg: qsTr("历史数据")
    StackView{
        id: dataView
        anchors.fill: parent
        initialItem: "./DataUI/DataMenu.qml"
    }
}
