import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
import Dx.Sequence 1.0

Page {
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

        Rectangle{
            id:updaterect
            width: 798
            height: 375
            radius: 22
            anchors.centerIn: parent

            Image {
                anchors.fill: parent
                source: "qrc:/images/upasswdbg.png"
            }

            Text{
                id:loldpassword
                width: 170
                x:100
                y:86
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("旧密码")
            }

            TextField{
                id: oldpassword
                anchors.left: loldpassword.right
                anchors.leftMargin: 30
                anchors.verticalCenter: loldpassword.verticalCenter
                width: 400
                height: 64
                background: Rectangle{border.color: "#cdcdcd";radius: 5;color: "#f5f5f5"}
            }

            Text{
                id:lpassword
                anchors.left: loldpassword.left
                anchors.leftMargin: 0
                width: loldpassword.width
                anchors.top: loldpassword.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("新密码")
            }

            TextField{
                id: password
                anchors.horizontalCenter: oldpassword.horizontalCenter
                anchors.verticalCenter: lpassword.verticalCenter
                width: 400
                height: 64
                background: Rectangle{border.color: "#cdcdcd";radius: 5;color: "#f5f5f5"}
            }

            Text{
                id:lppassword
                anchors.left: loldpassword.left
                anchors.leftMargin: 0
                width: loldpassword.width
                anchors.top: lpassword.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("确认新密码")
            }

            TextField{
                id: ppassword
                anchors.horizontalCenter: oldpassword.horizontalCenter
                anchors.verticalCenter: lppassword.verticalCenter
                width: 400
                height: 64
                background: Rectangle{border.color: "#cdcdcd";radius: 5;color: "#f5f5f5"}
            }
        }
        Text {
            id: mess
            anchors.top: updaterect.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: updaterect.horizontalCenter
        }
    }

    Bt2 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.right: parent.right
        anchors.rightMargin: 170
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        image: "qrc:/images/cancelbt.png"
        text:qsTr("取消")
        onClicked: {
            Sequence.setTitle("setup");
            mainView.pop();
        }
    }

    Bt2{
        id: btconfirm
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btCannel.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        textcolor: "#ffffff"
        image: "qrc:/images/confirmbt2.png"
        text:qsTr("确认")
        onClicked: {
            if (password.text != ppassword.text)
            {
                mess.text = qsTr("密码和确认密码不一致！");
            }
            else if(password.text == "")
            {
                mess.text = qsTr("密码不能为空！");
            }
            else
            {
                if (userModel.updatePassword(oldpassword.text,password.text))
                {
                    Sequence.setTitle("setup");
                    mainView.pop();
                }
                else
                    mess.text = qsTr("密码修改失败！")
            }
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("setup_passwd");
    }
}
