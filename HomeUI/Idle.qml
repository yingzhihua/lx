import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Sequence 1.0
import Dx.Global 1.0

Page {    
    property bool bErrorOpenDoor: false
    id: idle_page
    background: Rectangle{color: "#f5f5f5"}
/*
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
*/
    AnimatedImage{
        id:openDoor
        width: 230
        height: 230
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100
        source: "qrc:/images/opendoorAmi.gif"
        playing: false
        speed: 0.3
        MouseArea{
            anchors.fill: parent
            onClicked: {
                switchDoor()
            }
        }
    }    

    Text{
        id: boxstate

        text:qsTr("未检测到试剂盒")
        font.pixelSize: 45
        anchors.top: openDoor.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: openDoor.horizontalCenter
    }

    Text{
        id: sampleCode
        text:ExGlobal.sampleCode
        font.pixelSize: 30
        anchors.left: editsample.right
        anchors.leftMargin: 30
        anchors.verticalCenter: editsample.verticalCenter
        visible: false
        MouseArea{
            anchors.fill: parent
            onClicked: {
                queryDlg.show()
            }
        }
    }

    Image {
        id: editsample
        anchors.verticalCenter: openDoor.verticalCenter
        anchors.left: openDoor.right
        anchors.leftMargin: 200
        height: 75
        width: 70
        fillMode: Image.Pad
        source: "qrc:/images/Pen.png"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                queryDlg.show()
            }
        }
    }

    Bt2{
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

    Bt2{
        id: btExit
        anchors.bottom: btClose.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btClose.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        image: "qrc:/images/loginquit.png"
        text:qsTr("退出登录")
        onClicked: {
            mainView.pop();
            mainView.push("qrc:/HomeUI/Login.qml");
        }
    }

    Bt2{
        id: btReady
        anchors.bottom: btExit.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btExit.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 36
        textoffsety: -4
        image: "qrc:/images/loginquit.png"
        text:qsTr("试剂盒就绪")
        onClicked: {
            mainView.pop();
            mainView.push("qrc:/HomeUI/BoxReady.qml");
        }
    }

    ThreeQuery{
        id: queryDlg
        qtitle: qsTr("样本信息录入")
        qlable1: qsTr("样本号")
        qlable2: qsTr("样本信息")
        qlable3: qsTr("样本备注")
        qtext1: ExGlobal.sampleCode
        qtext2: ExGlobal.sampleInfo
        qtext3: ExGlobal.sampleRemark
        anchors.fill: parent
        onQueryAck: {
            console.log(res1,res2,res3);
            ExGlobal.sampleCode = res1;
            ExGlobal.sampleInfo = res2;
            ExGlobal.sampleRemark = res3;
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
    }

    SecondConfirm{
        id: confirmDlg
        anchors.fill: parent
        onQueryAck: {
            if (res == "confirm")
                ExGlobal.exClose()
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

    OneBtnQuery{
        id:promptClose
        anchors.fill: parent
        onCloseAck: {
            bErrorOpenDoor = true;
            busyIndicator.running = true;
            busyDes.visible = true;
            busyDes.text = qsTr("试剂盒错误，正在开仓");
            Sequence.sequenceDo(Sequence.Sequence_OpenBox);
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("idle");        
        Sequence.updateFooter(true,true,true);
        Sequence.actionDo("Query",3,0,0,0);
        Sequence.uiStage = Sequence.Stage_idle;
        if (Sequence.door)
            updateState.start();
        else
            updateState.stop();            
        if (Sequence.box)
            boxstate.text = qsTr("试剂盒就绪");
        else
            boxstate.text = qsTr("未检测到试剂盒");

        if (ExGlobal.projectMode() !== 0){
            btReady.visible = false;            
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
                    /*
                    busyIndicator.running = false;
                    busyDes.visible = false;
                    /*/
                    openDoor.playing = false
                    openDoor.enabled = true
                    //*/
                    updateState.start();
                }
            }            
            else if(result == Sequence.Result_CloseBox_ok)
            {
                updateState.stop();

                if (Sequence.box){
                    Sequence.sequenceDo(Sequence.Sequence_Pierce);
                    /*
                    busyIndicator.running = true;
                    busyDes.text = qsTr("正在检测试剂盒");
                    busyDes.visible = true;
                    //*/
                }
                else{
                    /*
                    busyIndicator.running = false;
                    busyDes.visible = false;
                    /*/
                    openDoor.playing = false
                    openDoor.enabled = true
                    //*/
                }
            }
            else if(result == Sequence.Result_Box_Valid)
            {
                mainView.pop();
                mainView.push("qrc:/HomeUI/BoxReady.qml");
            }
            else if(result == Sequence.Result_Box_Invalid)
            {                

                /*
                busyIndicator.running = false;
                busyDes.visible = false;
                /*/
                openDoor.playing = false
                openDoor.enabled = true
                //*/
                Sequence.setTitle("box_error");
                if (ExGlobal.projectMode() === 0)
                    promptDlg.show(qsTr("试剂盒错误，是否继续？"))
                else
                    promptClose.show(qsTr("试剂盒错误，取消测试"))
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
            if (Sequence.isIdle())
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
                /*
                busyIndicator.running = true;
                busyDes.text = qsTr("正在开仓");
                busyDes.visible = true;
                /*/
                openDoor.playing = true
                openDoor.enabled = false
                //*/
            }
        }
        else
        {
            if (Sequence.sequenceDo(Sequence.Sequence_CloseBox))
            {
                /*
                busyIndicator.running = true;
                busyDes.text = qsTr("正在合仓");
                busyDes.visible = true;
                /*/
                openDoor.playing = true
                openDoor.enabled = false
                //*/
            }
        }        
    }
}
