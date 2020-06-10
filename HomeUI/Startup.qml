import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

Page {
    property int titleChange: 0
    property bool pagequit: false
    id: start_page
    Image {
        id: appicon
        anchors.centerIn: parent
        scale: 2.4

        source: "qrc:/image/Dxlogo.png"
    }
    Component.onCompleted: {
        changeTimer.stop();
        //headerMsg.text=qsTr("检测试剂盒");
        changeTimer.start();
        tabBar.enabled = false;
        //appicon.visible = false;
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

    Text {
        id: boxTips
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -200
        font.pixelSize: 50
        text: qsTr("请取出机器内试剂盒，然后关闭舱门")
    }

    Image {
        id: openDoor
        width: 200
        height: 200
        anchors.horizontalCenter: boxTips.horizontalCenter
        anchors.top: boxTips.bottom
        anchors.topMargin: 100

        source: "qrc:/image/openDoor.png"
        MouseArea{
            anchors.fill: parent
            onClicked: {
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
        text: qsTr("text")
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
                    boxTips.text = qsTr("请取出机器内试剂盒，然后关闭舱门")
                }
                else if (Sequence.door){
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
    }

    Timer{
        id: changeTimer
        repeat: true
        interval: 500
        running: true
        onTriggered: {
            if (titleChange == 0)
                headerMsg.text=qsTr("系统启动");
            else if(titleChange == 1)
                headerMsg.text=qsTr("系统启动.");
            else if(titleChange == 2)
                headerMsg.text=qsTr("系统启动..");
            else if(titleChange == 3)
                headerMsg.text=qsTr("系统启动...");
            else if(titleChange == 4)
                headerMsg.text=qsTr("系统启动....");
            else if(titleChange == 5)
                headerMsg.text=qsTr("系统启动.....");
            else
                headerMsg.text=qsTr("系统启动......");
            titleChange++;
            if (titleChange>6)
                titleChange = 0;

        }
    }
}
