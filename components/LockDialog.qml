import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Window 2.12

import QtQuick 2.0

import Dx.Global 1.0

Rectangle {
    id: lockDialog
    visible: false
    color: Qt.rgba(0.5,0.5,0.5,0.8)
    Rectangle{
        z:2
        width: 675
        height: 409
        anchors.centerIn: parent
        //border.color: "darkgray"
        radius: 22

        Image {
            source: "qrc:/images/lockbg.png"
        }

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
            anchors.top: labelUser.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            width: 405
            height: 70
            echoMode: TextInput.Password
            font.pixelSize: 40
            placeholderTextColor: "#ffffff"
            placeholderText: qsTr("输入密码")
            background: Rectangle{
                //border.color: "darkgray"
                color: "#e5e5e5"
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
        login_password.focus = false;
        lockDialog.visible = false;
    }
    MouseArea{
        anchors.fill: parent
        onClicked: console.log("unlock fill")
    }
}
