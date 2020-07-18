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
    title: qsTr("闪量-华山")

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
                y: window.height - inputPanel.height -100
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
        height: 100
        color: "darkslateblue"
        Rectangle{
            id:testCompleted
            x:0
            y:0
            height: parent.height
            width:0
            color:"darkturquoise"
        }
        Rectangle{
            id:testunCompleted
            x:parent.width*0.5
            y:0
            height: parent.height
            width:0
            color:"gainsboro"
        }
        Text {
            id: headerMsg
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: parent.height*0.5
            color: "white"
            text: qsTr("系统启动");
        }
        Text {
            id: headerTitle
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: parent.height*0.5
            color: "white"
            text: ExGlobal.sysName
        }
    }

    /*
    header:Rectangle{
        id:headerdatetime
        height: 60
        width: parent.width
        //color:"gainsboro"
        Text{
            id:headertime
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.bottom: parent.bottom
            font.pixelSize: 40
            text:qsTr("17:22:14")
        }

        Text {
            id: headerdate
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            font.pixelSize: 40
            text: qsTr("2020-03-12")
        }
    }
*/
    header:Rectangle{
        id:headerdatetime
        height: 100
        width: parent.width

        Text{
            id:headertime
            color: "gray"
            anchors.right: parent.right
            anchors.rightMargin: 60
            anchors.top: parent.top
            anchors.topMargin: 10
            font.pixelSize: 60
            text:qsTr("17:22:14")
        }

        Text {
            id: headerdate
            color: "gray"
            anchors.top: headertime.bottom
            anchors.topMargin: -10
            anchors.horizontalCenter: headertime.horizontalCenter
            font.pixelSize: 30
            text: qsTr("2020-03-12")
        }
    }

    footer: TabBar{
        id:tabBar
        height:80
        width:parent.width
        currentIndex: 1

        background: Image{
            anchors.fill: parent
            smooth: true
            source:"qrc:/image/footerbk.jpg"
        }

        ListModel{
            id:footerModel
            ListElement{modelText:"设置";modelImgP:"qrc:";modelImg:"qrc";}
            ListElement{modelText:"主页";modelImgP:"qrc:";modelImg:"qrc";}
            ListElement{modelText:"数据";modelImgP:"qrc:";modelImg:"qrc";}
            //ListElement{modelText:"串口";modelImgP:"qrc:";modelImg:"qrc";}
        }
        Repeater{
            model:footerModel
            TabButton{
                height: tabBar.height
                contentItem: Text{
                    text:modelText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: tabBar.height*0.6
                    color:(model.index===tabBar.currentIndex)?"#148014":"#000000"
                }
                background: Rectangle{
                    height: 0
                }
            }
        }

        onCurrentIndexChanged: {
            mainView.clear()
            if (currentIndex == 0)
            {
                mainView.push("qrc:/SetupUI/SetupMenu.qml");
                headerRec.visible = false;
                headerdatetime.visible = false;
            }
            else if(currentIndex == 1)
            {
                if (Sequence.isTesting()){
                    if (ExGlobal.projectMode() === 0)
                        mainView.push("qrc:/HomeUI/TestProcessT.qml");
                    else
                        mainView.push("qrc:/HomeUI/TestProcess.qml");
                }
                else
                    mainView.push("qrc:/HomeUI/Idle.qml");
                headerRec.visible = true;
                headerdatetime.visible = true;
            }
            else if(currentIndex == 2)
            {
                mainView.push("qrc:/DataUI/DataMenu.qml");
                headerRec.visible = true;
                headerdatetime.visible = true;
            }

            console.debug("currIndex",currentIndex)
        }
    }
//*
    StackView{
        id: mainView
        anchors.fill: parent
        initialItem: ExGlobal.debug?"./HomeUI/Login.qml":"./HomeUI/Startup.qml"
    }
/*/
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Setup {
            id:setupPage
        }

        Home {
            id:homePage
        }

        Data {
            id:dataPage
        }

        onCurrentIndexChanged: {
            if (currentIndex == 0)
                headerMsg.text = setupPage.titlemsg
            else if(currentIndex == 1)
                headerMsg.text = homePage.titlemsg
            else if(currentIndex == 2)
                headerMsg.text = dataPage.titlemsg
        }
    }
//*/
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
            /*
            if (timeinited == false){
                timeinited = true;
                starttime.text = Qt.formatTime(new Date(),"hh:mm:ss");
            }
            else{
                t_runtime++;
                runtime.text = t_runtime;
            }
            */
        }
    }

    Connections{
        target: Sequence
        onErrOccur:{
            console.log("main.qml,result:"+error);
            if (ExGlobal.debug == false){
                err.errText = error;
                err.visible = true;
                tabBar.enabled = false;
            }
        }

        onTitleNotify:{
            console.log("onTitleNotify:"+titleparam+","+title);
            if (titleparam == 0){
                testCompleted.width = 0;
                testunCompleted.width = 0;
                headerMsg.text = title;
            }
            else if (titleparam == 1){
                headerRec.visible = false;
                headerdatetime.visible = false;
            }
            else if(titleparam == 2)
            {
                headerRec.visible = true;
                headerdatetime.visible = true;
            }
            else if (titleparam >= 100){
                var finish = titleparam - 100;
                testCompleted.width = finish*headerRec.width/1000;
                testunCompleted.x = testCompleted.width;
                testunCompleted.width = (1000-finish)*headerRec.width/1000;
                headerMsg.text = title;
            }
        }
    }

    Connections{
        target: ExGlobal
        onExglobalMessage:{
            if (code == 1 && ExGlobal.user != "NotLoggedIn"){
                console.log("onExglobalMessage",code)
                lockDialog.show()
                tabBar.enabled = false
            }
            else if(code == 2)
            {
                console.log("main.qml exit",code)
                lockDialog.exit()
                tabBar.enabled = true
                //inputPanel.active = false
            }
        }
    }

    Component.onCompleted: {
        //console.log("lx-model:"+testModel.rowCount());
        //console.log("lengxing",Qt.locale("en_US").nativeLanguageName)
        //VirtualKeyboardSettings.styleName = "retro"
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
