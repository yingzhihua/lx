import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Sequence 1.0
import Dx.Global 1.0

Page {
    property bool bDoorIsOpen: false
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
            onClicked: {
                console.log("openDoor,status:"+Sequence.door+"box,"+Sequence.box);                
                if (Sequence.door == false)
                {
                    if (Sequence.sequenceDo(Sequence.Sequence_OpenBox))
                    {
                        busyIndicator.running = true;
                        busyDes.text = qsTr("正在开仓");
                        busyDes.visible = true;
                    }
                }                
                /*
                else if (ExGlobal.projectMode() === 1 && Sequence.box == true){
                    Sequence.sequenceDo(Sequence.Sequence_Pierce);
                    busyIndicator.running = true;
                    busyDes.text = qsTr("正在检测试剂盒");
                    busyDes.visible = true;
                }                
                */
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

    Text{
        id: userName

        text:ExGlobal.user
        font.pixelSize: 40
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
    }

    Button {
        id: btClose        
        font.pixelSize: 30
        text: qsTr("关机")
        width: 200
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: ExGlobal.exClose()
    }

    Button {
        id: btExit        
        font.pixelSize: 30
        width: 200
        text: qsTr("退出登录")
        anchors.bottom: btClose.top
        anchors.bottomMargin: 20
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
        text: qsTr("试剂盒就绪")
        anchors.bottom: btExit.top
        anchors.bottomMargin: 20
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
        //home_page.titlemsg=qsTr("待机");
        //home_page.enableTabBar = true;

        //headerMsg.text = qsTr("待机");
        Sequence.changeTitle(qsTr("待机"));
        tabBar.enabled = true;

        Sequence.actionDo("Query",3,0,0,0);
        bDoorIsOpen = Sequence.door;
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

        if (ExGlobal.projectMode() === 1){
            btReady.visible = false;
            //btExit.visible = false;
        }

        console.log("idle onCompleted,box:"+Sequence.box+",door:"+Sequence.door);
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            if (result == Sequence.Result_OpenBox_ok)
            {
                bDoorIsOpen = true;
                busyIndicator.running = false;
                busyDes.visible = false;
                updateState.start();
            }            
            else if(result == Sequence.Result_CloseBox_ok)
            {
                bDoorIsOpen = false;                
                busyIndicator.running = false;
                busyDes.visible = false;                
                updateState.stop();
                if (Sequence.box)
                {
                    if (!Sequence.validBox())
                        ExGlobal.panelBoxIndexAddOne();
                    mainView.pop();
                    mainView.push("qrc:/HomeUI/BoxReady.qml");
                }                
            }
            else if(result == Sequence.Result_Pierce_Yes)
            {
                Sequence.sequenceDo(Sequence.Sequence_CloseBox);
                busyIndicator.running = true;
                busyDes.text = qsTr("正在合仓");
                busyDes.visible = true;
                console.log("close box after Pierce Yes");
            }
            else if(result == Sequence.Result_Pierce_No || result == Sequence.Result_Pierce_Damage || result == Sequence.Result_Pierce_Yes_NoQr)
            {                
                busyIndicator.running = false;
                busyDes.visible = true;
                if (result == Sequence.Result_Pierce_No)
                    busyDes.text = qsTr("检测试剂盒失败");
                else if (result == Sequence.Result_Pierce_Yes_NoQr)
                    busyDes.text = qsTr("二维码识别识别");
                else if (result == Sequence.Result_Pierce_Damage)
                    busyDes.text = qsTr("试剂盒已使用");
                console.log("close box after Pierce No");
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
            bDoorIsOpen = Sequence.door;
            if (Sequence.door)
                updateState.start();
            else
                updateState.stop();
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
}
