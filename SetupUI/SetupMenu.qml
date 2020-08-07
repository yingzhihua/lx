import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0

Rectangle {
    //property string titlemsg: qsTr("设置")
    id: setup_menu
    //anchors.fill: parent
    color: "darkgrey"
    ListModel{
        id:setModel
        ListElement{Eid: "SystemName"; name:qsTr("系统名称");info:"";qrc_res:"qrc:/SetupUI/SystemName.qml"}
        ListElement{Eid: "LockTime"; name:qsTr("锁屏时间");info:"";qrc_res:"qrc:/SetupUI/LockScreenSet.qml"}
        ListElement{Eid: "Language"; name:qsTr("语言");info:"";qrc_res:"qrc:/SetupUI/LanguageSet.qml"}
        ListElement{Eid: "AdminPassword"; name:qsTr("管理员密码");info:"";qrc_res:"qrc:/SetupUI/AdminPassword.qml"}
        ListElement{Eid: "Wlan"; name:qsTr("网络");info:"";qrc_res:"qrc:/SetupUI/WifiMenu.qml"}
        ListElement{Eid: "TimeSet"; name:qsTr("时间设置");info:"";qrc_res:"qrc:/SetupUI/TimeSet.qml"}
        ListElement{Eid: "User"; name:qsTr("用户帐号");info:"";qrc_res:"qrc:/SetupUI/User.qml"}
        ListElement{Eid: "Cali"; name:qsTr("机械校准");info:"";qrc_res:"qrc:/SetupUI/MachineCalibration.qml"}
        //ListElement{name:qsTr("温度控制");info:"";qrc_res:"qrc:/SetupUI/TempCtrl.qml"}
        //ListElement{name:qsTr("帕尔贴校准");info:"";qrc_res:"qrc:/SetupUI/TempCali.qml"}
        ListElement{Eid: "Camera"; name:qsTr("摄像头参数设置");info:"";qrc_res:"qrc:/SetupUI/CameraSetup.qml"}
        ListElement{Eid: "TestSetup"; name:qsTr("测试设置");info:"";qrc_res:"qrc:/SetupUI/TestSetup.qml"}
        ListElement{Eid: "TestLoop"; name:qsTr("循环测试");info:"";qrc_res:"qrc:/SetupUI/TestLoop.qml"}
        ListElement{Eid: "SystemParam"; name:qsTr("系统参数");info:"";qrc_res:"qrc:/SetupUI/SystemParam.qml"}
        ListElement{Eid: "Version"; name:qsTr("关于");info:"";qrc_res:"qrc:/SetupUI/Version.qml"}
    }

    ScrollView{
        anchors.fill: parent

        Column{
            Repeater{
                model: setModel

                Rectangle{
                    height: 80
                    width: setup_menu.width
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
                                mainView.push(qrc_res);
                        }
                    }
                }
            }

        }
    }

    Component.onCompleted: {
        setInfo("SystemName",ExGlobal.sysName);
        setInfo("LockTime",ExGlobal.getCaliParam("LockScreenTime")+qsTr(" 秒"));
        setInfo("Language",ExGlobal.sysLanguageName);
        setInfo("Version",ExGlobal.version);

        if (ExGlobal.user != "flashdx")
            removeItem("SystemParam");

        if (ExGlobal.user != "admin"){
            removeItem("SystemName");
            removeItem("Wlan");
            removeItem("TimeSet");
            removeItem("LockTime");
            removeItem("User");            
            setName("AdminPassword",qsTr("设置密码"))
        }

        if (ExGlobal.projectMode() !== 0){            
            //removeItem("Language");
            //removeItem("AdminPassword");
            //removeItem("Wlan");
            removeItem("TestSetup");            
            setRes("Version","qrc:/SetupUI/Version2.qml")
            removeItem("Cali");
            removeItem("Camera");
            removeItem("TestSetup");
            removeItem("TestLoop");
        }

        //headerMsg.text = qsTr("设置");
        Sequence.changeTitle(qsTr("设置"));
        Sequence.hideTitle(true);
    }

    Connections{
        target: ExGlobal
        onLockTimeChanged:{
            setModel.get(1).info = ExGlobal.getCaliParam("LockScreenTime")+qsTr(" 秒");
        }
        onSysNameChanged:{
            setModel.get(0).info = ExGlobal.sysName;
        }
        onUserChanged:{
            console.log("onUserChanged")
            if (ExGlobal.user != "admin"){
                removeItem("User");
                removeItem("SystemParam");
                setName("AdminPassword",qsTr("设置密码"))
            }
            else{
                setName("AdminPassword",qsTr("管理员密码"))
                insertItem("TimeSet",{Eid: "User", name:qsTr("用户帐号"),info:"",qrc_res:"qrc:/SetupUI/User.qml"})
            }
        }
        onSysLanguageCodeChanged:{
            console.log("onSysLanguageCodeChanged")
            setInfo("Language",ExGlobal.sysLanguageName);
        }
    }

    function removeItem(id){
        for (var i = 0; i < setModel.count; i++){
            if (setModel.get(i).Eid === id){
                setModel.remove(i,1);
            }
        }
    }
    function insertItem(id,item){
        for (var i = 0; i < setModel.count; i++){
            if (setModel.get(i).Eid === id){
                setModel.insert(i,item)
                return;
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

    function setRes(id,res){
        for (var i = 0; i < setModel.count; i++){
            if (setModel.get(i).Eid === id){
                setModel.get(i).qrc_res = res
            }
        }
    }
}
