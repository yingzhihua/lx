import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Window 2.12
import QtQuick.VirtualKeyboard 2.4
import QtQuick.VirtualKeyboard.Settings 2.2
import Dx.Global 1.0
import Dx.Sequence 1.0

import "./components"

ApplicationWindow {
    property string waitMsg: ""
    property int languageIndex: 0
    property bool timeinited: false
    property int t_runtime: 0
    id: window
    visible: true
    width: screen.width
    height: screen.height
    title: "闪量-华山"

    ListModel{
        id:titleModel
        ListElement{Eid: "login"; img:"qrc:/images/title_login.png"; name:qsTr("登录");color:"#5acedf";header:true}
        ListElement{Eid: "idle"; img:"qrc:/images/title_idle.png"; name:qsTr("待机");color:"#5acedf";header:true}
        ListElement{Eid: "boxready"; img:"qrc:/images/title_boxready.png"; name:qsTr("试剂盒就绪");color:"#5acedf";header:true}
        ListElement{Eid: "test"; img:"qrc:/images/title_test.png"; name:qsTr("测试中");color:"#5acedf";header:true}
        ListElement{Eid: "setup"; img:"qrc:/images/title_setup.png"; name:qsTr("设置");color:"#5acedf";header:false}
        ListElement{Eid: "setup_language"; img:"qrc:/images/title_language.png"; name:qsTr("语言");color:"#5acedf";header:false}
        ListElement{Eid: "setup_user"; img:"qrc:/images/title_user.png"; name:qsTr("用户帐号");color:"#5acedf";header:true}
        ListElement{Eid: "setup_useradd"; img:"qrc:/images/title_adduser.png"; name:qsTr("增加帐号");color:"#5acedf";header:true}
        ListElement{Eid: "setup_userupdate"; img:"qrc:/images/title_updateuser.png"; name:qsTr("修改帐号");color:"#5acedf";header:true}
        ListElement{Eid: "setup_lockscreen"; img:"qrc:/images/title_lockscreen.png"; name:qsTr("锁屏时间");color:"#5acedf";header:true}
        ListElement{Eid: "setup_passwd"; img:"qrc:/images/title_upasswd.png"; name:qsTr("修改密码");color:"#5acedf";header:true}
        ListElement{Eid: "setup_system"; img:"qrc:/images/title_system.png"; name:qsTr("系统设置");color:"#5acedf";header:true}
        ListElement{Eid: "setup_datetime"; img:"qrc:/images/title_datetime.png"; name:qsTr("时间");color:"#5acedf";header:true}
        ListElement{Eid: "setup_network"; img:"qrc:/images/title_network.png"; name:qsTr("有线网络");color:"#5acedf";header:true}
        ListElement{Eid: "setup_wifi"; img:"qrc:/images/title_wifi.png"; name:qsTr("无线网络");color:"#5acedf";header:true}
        ListElement{Eid: "startup"; img:"qrc:/images/title_startup.png"; name:qsTr("系统自检");color:"#5acedf";header:true}
        ListElement{Eid: "startup_error"; img:"qrc:/images/title_startup.png"; name:qsTr("系统启动");color:"#fc7050";header:true}
        ListElement{Eid: "box_error"; img:"qrc:/images/title_boxalarm.png"; name:qsTr("试剂盒错误");color:"#fc7050";header:true}
        ListElement{Eid: "datamenu"; img:"qrc:/images/title_datamenu.png"; name:qsTr("历史数据");color:"#5acedf";header:false}
        ListElement{Eid: "dataview"; img:"qrc:/images/title_data.png"; name:qsTr("历史数据");color:"#5acedf";header:true}
        ListElement{Eid: "error"; img:"qrc:/images/title_error.png"; name:qsTr("系统报警");color:"#fc7050";header:true}
    }

    LanguageDialog{
        id: languageDialog        
        x:600
        y:600
    }

    InputPanel {
        id: inputPanel
        externalLanguageSwitchEnabled: true
        z: 99
        x: window.width/3
        y: window.height
        width: window.width*2/3
        //x: 0
        //y: window.height
        //width: window.width
        //height: window.height*2/3

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height -150
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
        onExternalLanguageSwitch: {
            console.log("main.qml,result:",VirtualKeyboardSettings.locale);
            //languageDialog.show(["en_US","zh_CN"])
            if (VirtualKeyboardSettings.locale == "en_US")
                VirtualKeyboardSettings.locale = "zh_CN";
            else if(VirtualKeyboardSettings.locale == "zh_CN")
                VirtualKeyboardSettings.locale = "en_US";
            else
                VirtualKeyboardSettings.locale = "en_US";
        }
    }

    menuBar: Rectangle{
        id:headerRec
        width: parent.width
        height: 150
        color: "#59cddf"
        Rectangle{
            id:testunCompleted
            x:0
            y:0
            height: parent.height
            width:0
            color:"#c8e5ee"
        }

        Image {
            id: titleico
            x: 69
            y: 42
            height: 66
            width: 66
            fillMode: Image.Pad
            source: "qrc:/images/title_startup.png"
        }
        Text {
            id: headerMsg
            anchors.left: titleico.right
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 60
            color: "#323232"
            width: 1300
            clip:true
            elide:Text.ElideRight
            text: qsTr("系统启动");
        }
        Text {
            id: headerTitle
            anchors.right: parent.right
            anchors.rightMargin: 70
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 60
            color: "#323232"
            text: ExGlobal.sysName
        }
    }

    header:Rectangle{
        id:headerdatetime
        //height: 100
        height: 0
        width: parent.width

        Text{
            id:headeruser
            color: "#1c205b"
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.top: parent.top
            anchors.topMargin: 20
            font.pixelSize: 50
            text:ExGlobal.displayUser
        }

        Text{
            id:headertime
            color: "#b5b5b5"
            anchors.right: parent.right
            anchors.rightMargin: 70
            anchors.top: parent.top
            anchors.topMargin: 10
            font.pixelSize: 60
            text:"17:22:14"
        }

        Text {
            id: headerdate
            color: "#b5b5b5"
            anchors.top: headertime.bottom
            anchors.topMargin: -10
            anchors.horizontalCenter: headertime.horizontalCenter
            font.pixelSize: 30
            text: "2020-03-12"
        }
    }

    footer: Rectangle{
        width: parent.width
        height: 150
        color: "#313131"

        Image {
            id: setupbt
            x: 161
            y: 14
            height: 123
            width: 318
            fillMode: Image.Pad
            source: "qrc:/images/setuprelease.png"
            MouseArea{
                anchors.fill: parent
                onClicked: btclick(0)
            }
        }
        Text {
            id: setuptext
            anchors.centerIn: setupbt
            anchors.horizontalCenterOffset: 40
            anchors.verticalCenterOffset: -4
            font.pixelSize: 60
            color: "#A7A7A7"
            text: qsTr("设置")
        }
        Image {
            id: homebt
            x: 801
            y: 14
            height: 123
            width: 318
            fillMode: Image.Pad
            source: "qrc:/images/homerelease.png"
            MouseArea{
                anchors.fill: parent
                onClicked: btclick(1)
            }
        }
        Text {
            id: hometext
            anchors.centerIn: homebt
            anchors.horizontalCenterOffset: 40
            anchors.verticalCenterOffset: -4
            font.pixelSize: 60
            color: "#A7A7A7"
            text: qsTr("主页")
        }
        Image {
            id: databt
            x: 1440
            y: 14
            height: 123
            width: 318
            fillMode: Image.Pad
            source: "qrc:/images/datarelease.png"
            MouseArea{
                anchors.fill: parent
                onClicked: btclick(2)
            }
        }
        Text {
            id: datatext
            anchors.centerIn: databt
            anchors.horizontalCenterOffset: 40
            anchors.verticalCenterOffset: -4
            font.pixelSize: 60
            color: "#A7A7A7"
            text: qsTr("数据")
        }
    }

    StackView{
        id: mainView
        anchors.fill: parent
        initialItem: ExGlobal.debug?"qrc:/HomeUI/Login.qml":"qrc:/HomeUI/Startup.qml"
    }

    function btclick(index){
        mainView.clear()
        if (index === 0){
            setupbt.source = "qrc:/images/setuppress.png"
            homebt.source = "qrc:/images/homerelease.png"
            databt.source = "qrc:/images/datarelease.png"
            setuptext.color = "#ffffff"
            hometext.color = "#a7a7a7"
            datatext.color = "#a7a7a7"
            mainView.push("qrc:/SetupUI/SetupMenu.qml");
            headerdatetime.visible = false
        }
        else if(index === 1){
            setupbt.source = "qrc:/images/setuprelease.png"
            homebt.source = "qrc:/images/homepress.png"
            databt.source = "qrc:/images/datarelease.png"
            setuptext.color = "#a7a7a7"
            hometext.color = "#ffffff"
            datatext.color = "#a7a7a7"
            if (Sequence.isTesting()){
                if (ExGlobal.projectMode() === 0)
                    mainView.push("qrc:/HomeUI/TestProcessT.qml");
                else
                    mainView.push("qrc:/HomeUI/TestProcess.qml");
            }
            else
                mainView.push("qrc:/HomeUI/Idle.qml");
            headerdatetime.visible = true
        }
        else{
            setupbt.source = "qrc:/images/setuprelease.png"
            homebt.source = "qrc:/images/homerelease.png"
            databt.source = "qrc:/images/datapress.png"
            setuptext.color = "#a7a7a7"
            hometext.color = "#a7a7a7"
            datatext.color = "#ffffff"
            mainView.push("qrc:/DataUI/DataMenu.qml");
            headerdatetime.visible = false
        }
    }

    ErrPage{
        id:err
        visible: false
    }

    LockDialog{
        id:lockDialog
        anchors.fill: parent
    }

    Timer{
        repeat: true
        interval: 1000
        running: true
        onTriggered: {
            headertime.text = Qt.formatTime(new Date(),"hh:mm:ss")
            headerdate.text = Qt.formatDate(new Date(),"yyyy年MM月dd日")
        }
    }

    Connections{
        target: Sequence
        onErrOccur:{
            console.log("main.qml,result:"+error);
            if (ExGlobal.debug === false){
                err.errText = error;
                err.visible = true;
                Sequence.setTitle("error");
                Sequence.updateFooter(false,false,false);
            }
        }

        onTitleNotify:{
            console.log("onTitleNotify:"+titleparam+","+title);
            if (titleparam == 0){                              
                headerMsg.text = title;
            }
            else if (titleparam == 1){
                //headerRec.visible = false;
                headerdatetime.visible = false;
            }
            else if(titleparam == 2)
            {
                //headerRec.visible = true;
                headerdatetime.visible = true;
            }
            else if(titleparam == 3){
                headerRec.color = title;
            }
            else if(titleparam == 4){
                headerRec.color = "#59cddf";
            }
            else if(titleparam == 5){
                if (!Sequence.isTesting())
                {
                    testunCompleted.width = 0;
                    for(var i = 0; i < titleModel.count; i++){
                        if (titleModel.get(i).Eid === title){
                            titleico.source = titleModel.get(i).img;
                            headerRec.color = titleModel.get(i).color;
                            headerdatetime.visible = titleModel.get(i).header;
                            headerMsg.text = titleModel.get(i).name;
                        }
                    }
                }

            }
            else if (titleparam >= 100){
                var finish = titleparam - 100;
                testunCompleted.x = finish*headerRec.width/1000;
                testunCompleted.width = headerRec.width - testunCompleted.x;
                headerMsg.text = title;
            }
        }

        onFooterNotify:{
            if (setup === true)
                setupbt.enabled = true
            else
                setupbt.enabled = false
            if (home === true)
                homebt.enabled = true
            else
                homebt.enabled = false
            if (data === true)
                databt.enabled = true
            else
                databt.enabled = false

        }
    }

    Connections{
        target: ExGlobal
        onExglobalMessage:{
            if (code == 1 && ExGlobal.login()){
                console.log("onExglobalMessage",code)
                lockDialog.show()
                //tabBar.enabled = false
                Sequence.updateFooter(false,false,false);
            }
            else if(code == 2)
            {
                console.log("main.qml exit",code)
                lockDialog.exit()
                //tabBar.enabled = true
                Sequence.updateFooter(true,true,true);
                //inputPanel.active = false
            }
        }
    }

    Component.onCompleted: {
        //console.log("lx-model:"+testModel.rowCount());
        //console.log("lengxing",Qt.locale("en_US").nativeLanguageName)
        //VirtualKeyboardSettings.styleName = "retro"
        //VirtualKeyboardSettings.wordCandidateList.alwaysVisable = true;
    }

    onWaitMsgChanged: {
        if (waitMsg == "")
        {
            //busyIndicator.running = false;
            //waitText.visible = false;
        }
        else
        {
            //waitText.text = waitMsg;
            //waitText.visible = true;
            //busyIndicator.running = false;
            console.log("main.qml,result:"+waitMsg);
        }
    }
}
