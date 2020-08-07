import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Sequence 1.0
import Dx.Global 1.0

Page {    
    property bool bErrorOpenDoor: false
    id: idle_page

    Image {
        id: openDoor
        width: 150
        height: 150
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -60

        source: "qrc:/image/openDoor.png"
        MouseArea{
            anchors.fill: parent
            onClicked: switchDoor()
        }
    }

    Text{
        id: boxstate

        text:qsTr("未检测到试剂盒")
        font.pixelSize: 30
        anchors.top: openDoor.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: openDoor.horizontalCenter
    }

    Image {
        id: editsample
        anchors.left: openDoor.right
        anchors.leftMargin: 200
        anchors.bottom: openDoor.bottom
        anchors.bottomMargin: 30
        scale: 2
        source: "qrc:/image/editInfo.png"

        MouseArea{
            anchors.fill: parent
            onClicked: {
                queryDlg.show()
            }
        }
    }

    Text{
        id: sampleCode
        text:ExGlobal.sampleCode
        font.pixelSize: 30
        anchors.left: editsample.right
        anchors.leftMargin: 30
        anchors.verticalCenter: editsample.verticalCenter
        MouseArea{
            anchors.fill: parent
            onClicked: {
                queryDlg.show()
            }
        }
    }


    TwoQuery{
        id: queryDlg
        qtitle: qsTr("输入样本信息")
        qlable1: qsTr("样本号：")
        qlable2: qsTr("样本信息：")
        qtext1: ExGlobal.sampleCode
        qtext2: ExGlobal.sampleInfo
        anchors.fill: parent
        onQueryAck: {
            console.log(res1,res2);
            ExGlobal.sampleCode = res1;
            ExGlobal.sampleInfo = res2;
        }
    }

    TwoBtnQuery{
        id: promptDlg
        anchors.fill: parent
        btn1: qsTr("继续")
        btn2: qsTr("停止")
        onCloseAck: {
            if (res == 1){
                mainView.pop();
                mainView.push("qrc:/HomeUI/BoxReady.qml");
            }
            else{
                bErrorOpenDoor = true;
                busyIndicator.running = true;
                busyDes.visible = true;
                busyDes.text = qsTr("试剂盒错误，正在开仓");
                Sequence.sequenceDo(Sequence.Sequence_OpenBox);
            }
        }
    }

    Text{
        id: userName

        text:ExGlobal.user
        font.pixelSize: 40
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
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
        height: 70
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: confirmDlg.show(qsTr("确认关机？"))
    }

    Button {
        id: btExit        
        font.pixelSize: 30
        width: 200
        height: 70
        text: qsTr("退出登录")
        anchors.bottom: btClose.top
        anchors.bottomMargin: 60
        anchors.right: parent.right
        anchors.rightMargin: 20        
        onClicked: {
            mainView.pop();
            mainView.push("qrc:/HomeUI/Login.qml");
        }
    }

    Button {
        id: btReady
        font.pixelSize: 30
        visible: true
        width: 200
        height: 70
        text: qsTr("试剂盒就绪")
        anchors.bottom: btExit.top
        anchors.bottomMargin: 60
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            ExGlobal.panelBoxIndexAddOne();
            mainView.pop();
            mainView.push("qrc:/HomeUI/BoxReady.qml");
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
        anchors.topMargin: 40
        font.pixelSize: 30
        visible: false
        text: qsTr("text")
    }

    Component.onCompleted: {        
        Sequence.changeTitle(qsTr("待机"));
        tabBar.enabled = true;

        Sequence.actionDo("Query",3,0,0,0);        
        //*
        if (Sequence.door)
            updateState.start();
        else
            updateState.stop();
            //*/
        if (Sequence.box)
            boxstate.text = qsTr("试剂盒就绪");
        else
            boxstate.text = qsTr("未检测到试剂盒");

        if (ExGlobal.projectMode() !== 0){
            btReady.visible = false;
            //btExit.visible = false;
        }

        console.log("idle onCompleted,box:"+Sequence.box+",door:"+Sequence.door);
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            console.log("idle.qml,onSequenceFinish:"+result);
            if (result == Sequence.Result_OpenBox_ok)
            {
                if (bErrorOpenDoor)
                {
                    mainView.pop();
                    mainView.push("qrc:/HomeUI/E04.qml");
                }
                else
                {                    
                    busyIndicator.running = false;
                    busyDes.visible = false;
                    updateState.start();
                }
            }            
            else if(result == Sequence.Result_CloseBox_ok)
            {
                updateState.stop();

                if (Sequence.box){
                    Sequence.sequenceDo(Sequence.Sequence_Pierce);
                    busyIndicator.running = true;
                    busyDes.text = qsTr("正在检测试剂盒");
                    busyDes.visible = true;
                }
                else{
                    busyIndicator.running = false;
                    busyDes.visible = false;
                }
            }
            else if(result == Sequence.Result_Box_Valid)
            {
                mainView.pop();
                mainView.push("qrc:/HomeUI/BoxReady.qml");
            }
            else if(result == Sequence.Result_Box_Invalid)
            {                

                busyIndicator.running = false;
                busyDes.visible = false;
                promptDlg.msg = qsTr("试剂盒错误，是否继续？")
                promptDlg.show()

                //bErrorOpenDoor = true;
                //busyDes.text = qsTr("试剂盒错误，正在开仓");
                //Sequence.sequenceDo(Sequence.Sequence_OpenBox);
            }


            console.log("idle.qml,result:"+result);
        }

        onBoxStateChanged:{
            if (Sequence.box)
                boxstate.text = qsTr("试剂盒就绪");
            else
                boxstate.text = qsTr("未检测到试剂盒");
        }
        onDoorStateChanged:{            
            if (Sequence.door)
                updateState.start();
            else
                updateState.stop();
        }

        onDoorKeyPress:{
            console.log("idle.qml doorKeyPress");
            switchDoor();
        }
    }

    Timer{
        id: updateState
        repeat: true
        interval: 500
        running: false
        onTriggered: {
            console.log("idle query sensor state");
            Sequence.actionDo("Query",3,0,0,0);
        }
    }

    function switchDoor(){
        console.log("openDoor,status:"+Sequence.door+"box,"+Sequence.box);
        bErrorOpenDoor = false;
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
