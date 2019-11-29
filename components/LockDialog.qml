import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Window 2.12

import QtQuick 2.0

import Dx.Global 1.0
import Dx.UserMgr 1.0

Rectangle {
    id: lockDialog
    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)
    Rectangle{
        z:2
        width: 800
        height: 400
        anchors.centerIn: parent
        border.color: "darkgray"
        radius: 50

        Text{
            id:labelName
            anchors.verticalCenter : parent.verticalCenter
            anchors.verticalCenterOffset: -80
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -220
            width: 80
            height: 30
            text: qsTr("用户名：")
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 40
        }

        Text{
            id: labelUser
            anchors.verticalCenter: labelName.verticalCenter
            anchors.left: labelName.right
            anchors.leftMargin: 50
            width: 300
            height: 30
            text: ExGlobal.user
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 40
        }

        Text {
            id:labelPassword
            anchors.right: labelName.right
            anchors.rightMargin: 0
            anchors.top: labelName.bottom
            anchors.topMargin: 80
            width:80
            height:30
            text: qsTr("密码：")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 40
        }

        TextField{
            id:login_password
            anchors.verticalCenter: labelPassword.verticalCenter
            anchors.left: labelPassword.right
            anchors.leftMargin: 50
            width: 300
            height: 50            
            echoMode: TextInput.Password

            placeholderText: qsTr("请输入密码")
            background: Rectangle{
                border.color: "darkgray"
                radius: 5
            }
        }

        Image {
            id: unlock
            anchors.bottom: login_password.bottom
            anchors.bottomMargin: -10
            anchors.left: login_password.right
            anchors.leftMargin: 50
            source: "qrc:/image/unlock.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if (userModel.login(ExGlobal.user,login_password.text)){
                        console.log("unlock")
                        ExGlobal.qmlGlobalMessage(2);
                    }
                    else
                        labelMessage.text = qsTr("密码错误！")
                }
            }
        }

        Text{
            id:labelMessage
            anchors.left: login_password.right
            anchors.leftMargin: 0
            anchors.top: login_password.bottom
            anchors.topMargin: 60
            width: 80
            height: 30
            text: ""
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 40
        }
    }
    function show(){
        labelUser.text = ExGlobal.user
        login_password.text = ""
        labelMessage.text = ""
        lockDialog.visible = true
    }
    function exit(){
        lockDialog.visible = false;
    }
    MouseArea{
        anchors.fill: parent
        onClicked: console.log("unlock fill")
    }
}
