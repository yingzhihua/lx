import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0

Item {
    Text{
        id:labelOldPassword
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -100
        anchors.verticalCenterOffset: -50
        width: 80
        height: 30
        text: qsTr("旧密码：")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
    }

    TextField{
        id:inputOldPassword
        anchors.verticalCenter: labelOldPassword.verticalCenter
        anchors.left: labelOldPassword.right
        anchors.leftMargin: 50
        width:300
        height: 60
        verticalAlignment: Text.AlignBottom
        font.pixelSize: 40
        background: Rectangle{
            border.color: "darkgray"
            radius: 5
        }
    }

    Text{
        id:labelNewPassword
        anchors.right: labelOldPassword.right
        anchors.rightMargin: 0
        anchors.top: labelOldPassword.bottom
        anchors.topMargin: 60
        width: 80
        height: 30
        text: qsTr("新密码：")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
    }

    TextField{
        id:inputNewPassword
        anchors.verticalCenter: labelNewPassword.verticalCenter
        anchors.left: labelNewPassword.right
        anchors.leftMargin: 50
        width:300
        height: 60
        verticalAlignment: Text.AlignBottom
        font.pixelSize: 40
        background: Rectangle{
            border.color: "darkgray"
            radius: 5
        }
    }

    Button{
        id: btSet
        anchors.top: labelNewPassword.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: labelOldPassword.horizontalCenter
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
            if (userModel.updatePassword(inputOldPassword.text,inputNewPassword.text))
            setupView.pop();
            else
                labelMessage.text = qsTr("密码错误！")
        }
    }

    Text{
        id:labelMessage
        anchors.left: btSet.right
        anchors.leftMargin: 100
        anchors.top: btSet.bottom
        anchors.topMargin: 60
        width: 80
        height: 30
        text: ""
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
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
