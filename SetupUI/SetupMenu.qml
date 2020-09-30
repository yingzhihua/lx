import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0

Rectangle {    
    id: setup_menu    
    color: "#f5f5f5"
    ListModel{
        id:setModel
        ListElement{Eid: "SystemName"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("系统设置");info:"";qrc_res:"qrc:/SetupUI/SystemName.qml"}
        ListElement{Eid: "LockTime"; img:"qrc:/images/setupmenu_lock.png"; name:qsTr("锁屏时间");info:"";qrc_res:"qrc:/SetupUI/LockScreenSet.qml"}
        ListElement{Eid: "Language"; img:"qrc:/images/setupmenu_lauguage.png"; name:qsTr("语言");info:"";qrc_res:"qrc:/SetupUI/LanguageSet.qml"}
        ListElement{Eid: "AdminPassword"; img:"qrc:/images/setupmenu_password.png"; name:qsTr("修改密码");info:"";qrc_res:"qrc:/SetupUI/AdminPassword.qml"}
        ListElement{Eid: "Wlan"; img:"qrc:/images/setupmenu_wifi.png"; name:qsTr("无线网络");info:"";qrc_res:"qrc:/SetupUI/WifiMenu.qml"}
        ListElement{Eid: "Network"; img:"qrc:/images/setupmenu_net.png"; name:qsTr("有线网络");info:"";qrc_res:"qrc:/SetupUI/NetWork.qml"}
        ListElement{Eid: "TimeSet"; img:"qrc:/images/setupmenu_time.png"; name:qsTr("时间设置");info:"";qrc_res:"qrc:/SetupUI/TimeSet.qml"}
        ListElement{Eid: "User"; img:"qrc:/images/setupmenu_user.png"; name:qsTr("用户帐号");info:"";qrc_res:"qrc:/SetupUI/User.qml"}
        ListElement{Eid: "Cali"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("机械校准");info:"";qrc_res:"qrc:/SetupUI/MachineCalibration.qml"}
        //ListElement{name:qsTr("温度控制");info:"";qrc_res:"qrc:/SetupUI/TempCtrl.qml"}
        //ListElement{name:qsTr("帕尔贴校准");info:"";qrc_res:"qrc:/SetupUI/TempCali.qml"}
        ListElement{Eid: "Camera"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("摄像头参数设置");info:"";qrc_res:"qrc:/SetupUI/CameraSetup.qml"}
        ListElement{Eid: "TestSetup"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("测试设置");info:"";qrc_res:"qrc:/SetupUI/TestSetup.qml"}
        ListElement{Eid: "TestLoop"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("循环测试");info:"";qrc_res:"qrc:/SetupUI/TestLoop.qml"}
        ListElement{Eid: "SystemParam"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("系统参数");info:"";qrc_res:"qrc:/SetupUI/SystemParam.qml"}
        ListElement{Eid: "ProjectParam"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("工程参数");info:"";qrc_res:"qrc:/SetupUI/ProjectParam.qml"}
        ListElement{Eid: "Usb"; img:"qrc:/images/setupmenu_system.png"; name:qsTr("数据维护");info:"";qrc_res:"qrc:/SetupUI/UsbFile.qml"}
        ListElement{Eid: "Version"; img:"qrc:/images/setupmenu_about.png"; name:qsTr("关于");info:"";qrc_res:"qrc:/SetupUI/Version.qml"}
    }

    ScrollView{
        anchors.fill: parent
        //ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.width: 100
        Column{
            Repeater{
                model: setModel

                Rectangle{
                    height: 80
                    width: setup_menu.width
                    //color: (model.index%2 == 1)?"#ffffff":"lightgrey"
                    color: "#f5f5f5"
                    Image {
                        anchors.left: parent.left
                        anchors.leftMargin: 106
                        width: 50
                        height: 50
                        anchors.verticalCenter: parent.verticalCenter
                        fillMode: Image.Pad
                        source: img
                    }
                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 175
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 35
                        //font.bold: true
                        text: name
                    }
                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 200
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 35
                        color:"#636363"
                        text: info
                    }
                    Image {
                        id:row
                        anchors.right: parent.right
                        anchors.rightMargin: 120
                        width: 21
                        height: 37
                        anchors.verticalCenter: parent.verticalCenter
                        fillMode: Image.Pad
                        source: "qrc:/images/row1.png"
                    }

                    MouseArea{
                        anchors.fill: parent
                        onPressed: parent.color = "#DDDDDD"
                        onCanceled: parent.color = "#f5f5f5"
                        onReleased: {
                            parent.color = "#f5f5f5"
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
        setInfo("Language",ExGlobal.sysLanguageName);
        setInfo("Version",ExGlobal.version);
        setInfo("Network",ExGlobal.getIP());
        setInfo("Wlan",ExGlobal.getWIFIIP());
        setlocktimeinfo();

        if (ExGlobal.user != "flashdx")
            removeItem("SystemParam");

        if (ExGlobal.user != "admin"){
            removeItem("SystemName");
            removeItem("Wlan");
            removeItem("Network");
            removeItem("TimeSet");
            removeItem("LockTime");
            removeItem("User");            
            //setName("AdminPassword",qsTr("设置密码"))
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
            removeItem("ProjectParam")
        }

        //Sequence.changeTitle(qsTr("设置"));
        Sequence.setTitle("setup");
        //Sequence.hideTitle(true);
    }

    Connections{
        target: ExGlobal
        onLockTimeChanged:{
            setlocktimeinfo()
        }
        onLockscreenOpenChanged:{
            setlocktimeinfo()
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
        for (var i = 0; i < setModel.count; i++)
        {
            if (setModel.get(i).Eid === item.Eid)
                return;
        }

        for (i = 0; i < setModel.count; i++){
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

    function setlocktimeinfo(){
        if (!ExGlobal.lockscreenOpen)
            setInfo("LockTime",qsTr("关闭"));
        else if(ExGlobal.lockTime == 0)
            setInfo("LockTime","1 "+qsTr("分钟"));
        else if(ExGlobal.lockTime == 1)
            setInfo("LockTime","3 "+qsTr("分钟"));
        else if(ExGlobal.lockTime == 2)
            setInfo("LockTime","5 "+qsTr("分钟"));
        else if(ExGlobal.lockTime == 3)
            setInfo("LockTime","10 "+qsTr("分钟"));
        else if(ExGlobal.lockTime == 4)
            setInfo("LockTime","15 "+qsTr("分钟"));
        else
            setInfo("LockTime","30 "+qsTr("分钟"));
    }
}
