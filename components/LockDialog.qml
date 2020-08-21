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
        width: 675
        height: 404
        anchors.centerIn: parent
        //border.color: "darkgray"
        radius: 22

        Text{
            id: labelUser
            anchors.horizontalCenter: parent.horizontalCenter
            y:70
            text: ExGlobal.user
            //horizontalAlignment: Text.AlignLeft
            //verticalAlignment: Text.AlignVCenter
            font.pixelSize: 50
        }

        TextField{
            id:login_password
            anchors.top: labelUser.top
            anchors.topMargin: 84
            anchors.horizontalCenter: parent.horizontalCenter
            width: 404
            height: 70
            echoMode: TextInput.Password
            font.pixelSize: 40
            placeholderText: qsTr("输入密码")
            background: Rectangle{
                //border.color: "darkgray"
                color: "#e6e6e6"
                radius: 5
            }
        }

        Image {
            id: unlock
            anchors.horizontalCenter: parent.horizontalCenter
            y:275
            width: 67
            height: 67
            source: "qrc:/images/lockarrow.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    var loginresult = userModel.login(ExGlobal.user,login_password.text);
                    console.log("lock",loginresult);
                    if (loginresult === 0)
                    {
                        console.log("unlock")
                        ExGlobal.qmlGlobalMessage(2);
                    }
                    else
                        labelMessage.text = qsTr("密码错误！");
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
