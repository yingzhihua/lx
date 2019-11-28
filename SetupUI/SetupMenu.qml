import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Rectangle {
    property string titlemsg: qsTr("设置")    
    id: setup_menu
    color: "darkgrey"
    ListModel{
        id:setModel
        ListElement{Eid: "SystemName"; name:qsTr("系统名称");info:"";qrc_res:"qrc:/SetupUI/SystemName.qml"}
        ListElement{Eid: "LockTime"; name:qsTr("锁屏时间");info:"";qrc_res:"qrc:/SetupUI/LockScreenSet.qml"}
        ListElement{Eid: "Language"; name:qsTr("语言");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{Eid: "AdminPassword"; name:qsTr("管理员密码");info:"";qrc_res:"qrc:/SetupUI/AdminPassword.qml"}
        ListElement{Eid: "Wlan"; name:qsTr("网络");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{Eid: "TimeSet"; name:qsTr("时间设置");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{Eid: "User"; name:qsTr("用户帐号");info:"";qrc_res:"qrc:/SetupUI/User.qml"}
        ListElement{Eid: "Cali"; name:qsTr("机械校准");info:"";qrc_res:"qrc:/SetupUI/MachineCalibration.qml"}
        //ListElement{name:qsTr("温度控制");info:"";qrc_res:"qrc:/SetupUI/TempCtrl.qml"}
        //ListElement{name:qsTr("帕尔贴校准");info:"";qrc_res:"qrc:/SetupUI/TempCali.qml"}
        ListElement{Eid: "Camera"; name:qsTr("摄像头参数设置");info:"";qrc_res:"qrc:/SetupUI/CameraSetup.qml"}
        ListElement{Eid: "TestSetup"; name:qsTr("测试设置");info:"";qrc_res:"qrc:/SetupUI/TestSetup.qml"}
        ListElement{Eid: "Version"; name:qsTr("版本号");info:"";qrc_res:"qrc:/SetupUI/Version.qml"}
    }

    ScrollView{
        anchors.fill: parent

        Column{
            Repeater{
                model: setModel

                Rectangle{
                    height: 80
                    width: dataPage.width
                    //color: (model.index%2 == 1)?"#ffffff":"lightgrey"
                    color: "darkseagreen"
                    Rectangle{
                        height: 1
                        color: "white"
                        anchors.top: parent.top
                        width:parent.width
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 30
                        text: name
                    }
                    Text {
                        x:parent.width*0.4
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 30
                        text: info
                    }
                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 30
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 30
                        text: ">"
                    }
                    Rectangle{
                        height: 1
                        color: "black"
                        anchors.bottom: parent.bottom
                        width:parent.width
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if (qrc_res != "")
                                setupView.push(qrc_res);
                        }
                    }
                }
            }

        }
    }

    onTitlemsgChanged: headerMsg.text = titlemsg;

    Component.onCompleted: {
        setInfo("SystemName",ExGlobal.sysName);
        setInfo("LockTime",ExGlobal.getCaliParam("LockScreenTime")+qsTr(" 秒"));
        setInfo("Language",ExGlobal.sysLanguageName);
        setInfo("Version",ExGlobal.version);

        if (ExGlobal.user != "admin"){
            removeItem("User");
            setName("AdminPassword",qsTr("设置密码"))
        }

        //removeItem("Language");
        //removeItem("Wlan");
    }

    Connections{
        target: ExGlobal
        onLockTimeChanged:{
            setModel.get(1).info = ExGlobal.getCaliParam("LockScreenTime")+qsTr(" 秒");
        }
        onSysNameChanged:{
            setModel.get(0).info = ExGlobal.sysName;
        }
    }

    function removeItem(id){
        for (var i = 0; i < setModel.count; i++){
            if (setModel.get(i).Eid === id){
                setModel.remove(i,1);
            }
        }
    }

    function setInfo(id,info){
        for (var i = 0; i < setModel.count; i++){
            if (setModel.get(i).Eid === id){
                setModel.get(i).info = info
            }
        }
    }

    function setName(id,name){
        for (var i = 0; i < setModel.count; i++){
            if (setModel.get(i).Eid === id){
                setModel.get(i).name = name
            }
        }
    }
}
