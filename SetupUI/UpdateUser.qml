import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

        Rectangle{
            width: 811
            height: 485
            radius: 22
            anchors.centerIn: parent

            Image {
                anchors.fill: parent
                source: "qrc:/images/loginbk.png"
            }
            Text{
                id: luser
                x:160
                width: 100
                y:50
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("帐号")
            }

            TextField{
                id: user
                anchors.left: luser.right
                anchors.leftMargin: 30
                anchors.verticalCenter: luser.verticalCenter
                width: 400
                enabled: false
                text:userModel.getCurName()
            }

            Text{
                id:ldisplayName
                width: 100
                anchors.left: luser.left
                anchors.leftMargin: 0
                anchors.top: luser.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("显示名")
            }

            TextField{
                id: displayName
                anchors.horizontalCenter: user.horizontalCenter
                anchors.verticalCenter: ldisplayName.verticalCenter
                width: 400
                text: userModel.getCurDisplayName()
            }

            Text{
                id:lpassword
                anchors.left: luser.left
                anchors.leftMargin: 0
                width: 100
                anchors.top: ldisplayName.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("密码")
            }

            TextField{
                id: password
                anchors.horizontalCenter: user.horizontalCenter
                anchors.verticalCenter: lpassword.verticalCenter
                width: 400
            }

            Text{
                id:lppassword
                anchors.left: luser.left
                anchors.leftMargin: 0
                width: 100
                anchors.top: lpassword.bottom
                anchors.topMargin: 30
                font.pixelSize: 40
                horizontalAlignment:Text.AlignRight
                text:qsTr("确认密码")
            }

            TextField{
                id: ppassword
                anchors.horizontalCenter: user.horizontalCenter
                anchors.verticalCenter: lppassword.verticalCenter
                width: 400
            }

            CheckBox{
                id: checker
                anchors.top: ppassword.bottom
                anchors.topMargin: 30
                anchors.left: ppassword.left
                anchors.leftMargin: 0
                text:qsTr("审核权限")
                checked: userModel.getCurUserType()&2
            }
        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
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
        image: "qrc:/images/loginquit.png"
        text:qsTr("确认修改")
        onClicked: {
            mainView.pop();
        }
    }

    Component.onCompleted: {

    }
}
