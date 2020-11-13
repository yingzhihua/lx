import QtQuick 2.12
import QtQuick.Controls 2.12

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
/*
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
            */

            ComboBox {
                id: control
                y: 141
                width: 640
                height: 70
                anchors.horizontalCenter: parent.horizontalCenter
                model: userModel.getLogUser()
                currentIndex: -1
                delegate: ItemDelegate {
                    width: 640
                    contentItem: Text {
                        text: modelData
                        //color: "#21be2b"
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    highlighted: control.highlightedIndex === index
                }
/*
                indicator: Canvas {
                    id: canvas
                    x: control.width - width
                    y: 0
                    width: 100
                    height: control.height
                    contextType: "2d"
                    Connections {
                        target: control
                        onPressedChanged: canvas.requestPaint()
                    }
                    onPaint: {
                        var w = 30;
                        var h = 26;
                        context.reset();
                        context.moveTo((width-w)/2,(height-h)/2);
                        context.lineTo((width+w)/2,(height-h)/2);
                        context.lineTo(width / 2, (height+h)/2);
                        context.closePath();
                        context.fillStyle = control.pressed ? "#17a81a" : "#21be2b";
                        context.fill();
                    }
                }
                */
                indicator: Image {
                    id: xiala
                    x: control.width -width
                    y: 0
                    width: 51
                    height: 71
                    source: "qrc:/images/xiala.png"
                }

                contentItem: TextField {
                    id: login_name
                    width: control.width - xiala.width
                    text: control.displayText
                    font: control.font
                    color: control.pressed ? "#17a81a" : "#210000"
                    verticalAlignment: Text.AlignVCenter
                    placeholderText: qsTr("用户名")
                    placeholderTextColor:"#ffffff"
                    background: Rectangle{radius: 5; color: "#e6e6e6"}
                }
                background: Rectangle {
                    color: "#e6e6e6"
                    radius: 5
                }
                popup: Popup {
                    y: control.height - 1
                    width: control.width
                    implicitHeight: contentItem.implicitHeight
                    padding: 1

                    contentItem: ListView {
                        clip: true
                        implicitHeight: contentHeight
                        model: control.popup.visible ? control.delegateModel : null
                        currentIndex: control.highlightedIndex

                        ScrollIndicator.vertical: ScrollIndicator { }
                    }

                    background: Rectangle {
                        border.color: "#e6e6e6"
                        radius: 2
                    }
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
            /*
            Component{
                id:listDelegate
                Text{
                    id:listname
                    x:0
                    width: 400
                    height: 32
                }
            }
            */
/*
            ListView{
                id:useList
                width: login_name.width
                height: 100
                anchors.left: login_name.left
                anchors.leftMargin: 0
                anchors.top: login_name.bottom
                anchors.topMargin: 0
                model: Sequence.TestList
            }
            */
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
        Sequence.checkParam(false);
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
            console.log("login:",login_name.text,ExGlobal.user,"depth=",mainView.depth);
            mainView.pop(mainView.initialItem);
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
