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
            color:"white"
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
            text:ExGlobal.user
        }

        Text{
            id:headertime
            color: "#b5b5b5"
            anchors.right: parent.right
            anchors.rightMargin: 70
            anchors.top: parent.top
            anchors.topMargin: 10
            font.pixelSize: 60
            text:qsTr("17:22:14")
        }

        Text {
            id: headerdate
            color: "#b5b5b5"
            anchors.top: headertime.bottom
            anchors.topMargin: -10
            anchors.horizontalCenter: headertime.horizontalCenter
            font.pixelSize: 30
            text: qsTr("2020-03-12")
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
            source: "qrc:/images/setup.png"
            MouseArea{
                anchors.fill: parent
                onClicked: btclick(0)
            }
        }
        Image {
            id: homebt
            x: 801
            y: 14
            height: 123
            width: 318
            fillMode: Image.Pad
            source: "qrc:/images/homebt.png"
            MouseArea{
                anchors.fill: parent
                onClicked: btclick(1)
            }
        }
        Image {
            id: databt
            x: 1440
            y: 14
            height: 123
            width: 318
            fillMode: Image.Pad
            source: "qrc:/images/data.png"
            MouseArea{
                anchors.fill: parent
                onClicked: btclick(2)
            }
        }
    }

    StackView{
        id: mainView
        anchors.fill: parent
        initialItem: ExGlobal.debug?"qrc:/HomeUI/Idle.qml":"qrc:/HomeUI/Startup.qml"
    }

    function btclick(index){
        mainView.clear()
        if (index === 0){
            setupbt.source = "qrc:/images/setupbt.png"
            homebt.source = "qrc:/images/home.png"
            databt.source = "qrc:/images/data.png"
            mainView.push("qrc:/SetupUI/SetupMenu.qml");
            headerdatetime.visible = false
        }
        else if(index === 1){
            setupbt.source = "qrc:/images/setup.png"
            homebt.source = "qrc:/images/homebt.png"
            databt.source = "qrc:/images/data.png"
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
            setupbt.source = "qrc:/images/setup.png"
            homebt.source = "qrc:/images/home.png"
            databt.source = "qrc:/images/databt.png"
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
                setupbt.enabled = false;
                homebt.enabled = false;
                databt.enabled = false;
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
            if (code == 1 && ExGlobal.user != "NotLoggedIn"){
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
