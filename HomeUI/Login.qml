import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    id: login_page
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

            Image {
                id:login_logo
                y:49
                anchors.horizontalCenter: parent.horizontalCenter
                source: "qrc:/images/loginlogo.png"
            }

            TextField{
                id:login_name
                y:141
                anchors.horizontalCenter: parent.horizontalCenter
                width:640
                height: 70
                verticalAlignment: Text.AlignVCenter

                placeholderText: qsTr("用户名")
                placeholderTextColor:"#ffffff"
                background: Rectangle{
                    //border.color: "darkgray"
                    radius: 5
                    color: "#e6e6e6"
                }
            }

            TextField{
                id:login_password
                y:235
                anchors.horizontalCenter: parent.horizontalCenter
                width:640
                height: 70
                verticalAlignment: Text.AlignVCenter
                echoMode: TextInput.Password
                placeholderText: qsTr("密码")
                placeholderTextColor:"#ffffff"
                background: Rectangle{
                    //border.color: "darkgray"
                    radius: 5
                    color: "#e6e6e6"
                }
            }

            Bt2{
                id: btlogin
                y: 347
                anchors.horizontalCenter: parent.horizontalCenter
                height: 91
                width: 315
                image: "qrc:/images/loginbt.png"
                text: qsTr("登录")
                textcolor: "#ffffff"
                textoffsetx: 35
                textoffsety: -4
                onClicked: login()
            }

            Text{
                id:labelMessage
                anchors.left: btlogin.right
                anchors.leftMargin: 200
                anchors.bottom: btlogin.bottom
                anchors.bottomMargin: 0
                width: 80
                height: 30
                text: ""
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 40
            }
        }

        SecondConfirm{
            id: confirmDlg
            anchors.fill: parent
            onQueryAck: {
                if (res == "confirm")
                    ExGlobal.exClose()
            }
        }

        Bt2 {
            id: btClose
            y: 754
            x: 1450
            height: 106
            width: 299
            textoffsetx: 50
            textoffsety: -4
            image: "qrc:/images/btclose.png"
            text: qsTr("关机")
            onClicked: confirmDlg.show(qsTr("您将关机？"))
        }
    }

    Component.onCompleted: {        
        //headerMsg.text = qsTr("登录");
        Sequence.uiStage = Sequence.Stage_login;
        Sequence.setTitle("login");
        Sequence.updateFooter(false,false,false);
        labelMessage.text = "";
        console.log("Login Completed");
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            console.log("result:"+result);
            //login_name.text = "result:"+result;
        }
    }

    function login(){                
        console.log("login_name:",ExGlobal.adminPassword)
        var loginresult = userModel.login(login_name.text,login_password.text);
        if (loginresult === 0)
        {
            console.log("login:",login_name.text,ExGlobal.user);
            mainView.pop();
            mainView.push("qrc:/HomeUI/Idle.qml");
        }
        else if (loginresult === 1)
            labelMessage.text = qsTr("密码错误！");
        else
            labelMessage.text = qsTr("帐号不存在！");
    }

    function createtable(){
        //SqliteMgr.init();
        //ParmMgr.init();
    }
}
