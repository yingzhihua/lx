import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

Page {
    property int titleChange: 0
    property bool pagequit: false
    id: start_page
    background: Rectangle{color: "#f5f5f5"}

    Image {
        id: appicon
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100
        source: "qrc:/images/startup_logo.png"
    }

    Component.onCompleted: {
        changeTimer.stop();        
        //changeTimer.start();
        Sequence.setTitle("startup");
        Sequence.updateFooter(false,false,false);
        openDoor.visible = false;
        boxTips.visible = false;
        Sequence.sequenceInit();
        if (ExGlobal.projectMode() === 0)
        {
            Sequence.sequenceDo(Sequence.Sequence_SelfCheck);            
        }
        else
            Sequence.actionDo("Query",3,0,0,0);
    }

    Image {
        id: openDoor
        width: 219
        height: 224
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100
        source: "qrc:/images/opendoor.png"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                switchDoor()
            }
        }
    }

    Text {
        id: boxTips
        anchors.horizontalCenter: openDoor.horizontalCenter
        anchors.top: openDoor.bottom
        anchors.topMargin: 40
        font.pixelSize: 45
        width: 300
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WrapAnywhere
        text: qsTr("请取出机器内试剂盒，然后关闭舱门")
    }

    BusyIndicator{
        id:busyIndicator
        anchors.centerIn: parent
        implicitWidth: 200
        implicitHeight: 200
        opacity: 1.0
        running: false
        MouseArea{
            anchors.fill: parent
            enabled: busyIndicator.running?true:false
        }
    }
    Text {
        id: busyDes
        anchors.horizontalCenter: busyIndicator.horizontalCenter
        anchors.top: busyIndicator.bottom
        anchors.topMargin: 20
        visible: false        
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            if (pagequit) return;
            if (result == Sequence.Result_SelfCheck_ok)
            {
                changeTimer.stop();
                pagequit = true;
                //stackView.pop();
                mainView.pop();
                mainView.push("qrc:/HomeUI/Login.qml");
            }
            if (result == Sequence.Result_OpenBox_ok)
            {
                busyIndicator.running = false;
                busyDes.visible = false;
            }
            else if(result == Sequence.Result_CloseBox_ok)
            {
                busyIndicator.running = false;
                busyDes.visible = false;
                Sequence.actionDo("Query",3,0,0,0);
            }
            else if (result == Sequence.Result_Simple_ok){
                if (Sequence.box){
                    Sequence.setTitle("startup_error")
                    boxTips.text = qsTr("请取出试剂盒 然后关闭舱门")
                }
                else if (Sequence.door){
                    Sequence.setTitle("startup_error")
                    boxTips.text = qsTr("请关闭舱门")
                }

                if (Sequence.box || Sequence.door){
                    appicon.visible = false;
                    openDoor.visible = true;
                    boxTips.visible = true;
                }
                else {
                    appicon.visible = true;
                    openDoor.visible = false;
                    boxTips.visible = false;
                    Sequence.setTitle("startup")
                    Sequence.sequenceDo(Sequence.Sequence_SelfCheck);
                    changeTimer.start();
                }
                console.log("box",Sequence.box,"door",Sequence.door)
            }

            console.log("Startup.qml,result:"+result);
        }
        onBoxStateChanged:{
            /*
            //if (ExGlobal.projectMode() === 1)
            if (Sequence.box){
                console.log("Startup.qml,Prompt for box");
                appicon.visible = false;
                openDoor.visible = true;
                boxTips.visible = true;
            }
            else if (!Sequence.door){
                appicon.visible = true;
                openDoor.visible = false;
                boxTips.visible = false;
                Sequence.sequenceDo(Sequence.Sequence_SelfCheck);
                changeTimer.start();
            }
            console.log("box",Sequence.box,"door",Sequence.door)
            */
        }
        onDoorKeyPress:{
            if (openDoor.visible)
                switchDoor();
        }
    }

    Timer{
        id: changeTimer
        repeat: true
        interval: 500
        running: true
        onTriggered: {
            headerMsg.text=qsTr("系统启动");
            if(titleChange == 1)
                headerMsg.text+=".";
            else if(titleChange == 2)
                headerMsg.text+="..";
            else if(titleChange == 3)
                headerMsg.text+="...";
            else if(titleChange == 4)
                headerMsg.text+="....";
            else if(titleChange == 5)
                headerMsg.text+=".....";
            else
                headerMsg.text+="......";
            titleChange++;
            if (titleChange>6)
                titleChange = 0;

        }
    }

    function switchDoor(){
        if (Sequence.door == false)
        {
            if (Sequence.sequenceDo(Sequence.Sequence_OpenBox))
            {
                busyIndicator.running = true;
                busyDes.text = qsTr("正在开仓");
                busyDes.visible = true;
            }
        }
        else
        {
            if (Sequence.sequenceDo(Sequence.Sequence_CloseBox))
            {
                busyIndicator.running = true;
                busyDes.text = qsTr("正在合仓");
                busyDes.visible = true;
            }
        }
    }
}
