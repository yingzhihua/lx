import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
Item {
    Text{
        id:labelName
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -100
        anchors.verticalCenterOffset: -50
        width: 80
        height: 30
        text: qsTr("系统名称：")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
    }

    TextField{
        id:inputName
        anchors.verticalCenter: labelName.verticalCenter
        anchors.left: labelName.right
        anchors.leftMargin: 50
        width:300
        height: 50
        verticalAlignment: Text.AlignBottom
        font.pixelSize: 20
        background: Rectangle{
            border.color: "darkgray"
            radius: 5
        }


    }
    Button{
        id: btSave
        anchors.top: labelName.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: labelName.horizontalCenter
        anchors.horizontalCenterOffset: 100
        width: 150
        height: 50

        contentItem: Text {
                  text: qsTr("设置")
                  color: "white"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
                  font.pixelSize: 30
              }
        background: Rectangle{
            color: "darkgray"
            radius: 5
        }
        onClicked: {            
            ExGlobal.sysName = inputName.text;
            setupView.pop();
        }
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

    Component.onCompleted: {
        inputName.text = ExGlobal.sysName
    }

}
