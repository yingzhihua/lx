import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.UserMgr 1.0
import Dx.Global 1.0

import "../components"
Page {
    id: login_page
    Rectangle{
        width: 400
        height: 600
        anchors.centerIn: parent

        Image {
            id:login_logo
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            scale: 3
            source: "qrc:/image/LOGO.png"
        }
//*
        Text{
            x:47
            anchors.top: login_logo.bottom
            anchors.topMargin: 120
            width: 80
            height: 30
            text: qsTr("用户名：")
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 40
        }

        TextField{
            id:login_name
            x:137
            anchors.top: login_logo.bottom
            anchors.topMargin: 120
            width:300
            height: 50
            verticalAlignment: Text.AlignVCenter

            placeholderText: qsTr("请输入用户名")
            background: Rectangle{
                border.color: "darkgray"
                radius: 5
            }
        }

        Text {
            x:47
            anchors.top: login_name.bottom
            anchors.topMargin: 50
            width:80
            height:30
            text: qsTr("密码：")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 40
        }

        TextField{
            id:login_password
            x:137
            anchors.top: login_name.bottom
            anchors.topMargin: 50
            width: 300
            height: 50
            echoMode: TextInput.Password

            placeholderText: qsTr("请输入密码")
            background: Rectangle{
                border.color: "darkgray"
                radius: 5
            }
        }

        Button{
            id:login_btlogin
            x:150
            anchors.top: login_password.bottom
            anchors.topMargin: 80
            width: 200
            height: 80

            contentItem: Text {
                      text: qsTr("登录")
                      color: "white"
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter
                      elide: Text.ElideRight
                      font.pixelSize: 50
                  }
            background: Rectangle{
                color: "darkgray"
                radius: 5
            }
            onClicked: login()
        }

        Text{
            id:labelMessage
            anchors.left: login_btlogin.right
            anchors.leftMargin: 200
            anchors.bottom: login_btlogin.bottom
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

    Button {
        id: btClose
        font.pixelSize: 30
        text: qsTr("关机")
        width: 200
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        //onClicked: ExGlobal.exClose()
        onClicked: confirmDlg.show(qsTr("确认关机？"))
    }

    Component.onCompleted: {
        //home_page.titlemsg=qsTr("登录");
        //home_page.enableTabBar = false;
        headerMsg.text = qsTr("登录");
        tabBar.enabled = false;
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
