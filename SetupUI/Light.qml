import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0
import "../components"
Page {
    Rectangle{
        id:viewWin
        x:10
        y:10
        width:  1040//652
        height: 780//490
        color:"lightskyblue"

        Text{
            id:viewText
            font.pixelSize: 40
            anchors.centerIn: parent
        }
        Image {
            id: cameraBox
            anchors.centerIn: parent
            width: parent.width - 4
            height: parent.height - 4
            source: ""
        }
    }

    Button{
        id:toWork
        anchors.left: viewWin.left
        anchors.leftMargin: 50
        anchors.top: viewWin.bottom
        anchors.topMargin: 30
        text: qsTr("舱门到工作位")
        font.pixelSize: 30
        onClicked: {
            doingBox.show(qsTr("舱门移动到工作位"))
            Sequence.sequenceDo(Sequence.Sequence_DoorToWork);
        }
    }

    Button{
        id:toReset
        anchors.left: toWork.right
        anchors.leftMargin: 100
        anchors.top: viewWin.bottom
        anchors.topMargin: 30
        text: qsTr("舱门回硬件位")
        font.pixelSize: 30
        onClicked: {
            doingBox.show(qsTr("舱门回到硬件零点"))
            Sequence.sequenceDo(Sequence.Sequence_DoorFromWork);
        }
    }

    Text {
        id: lbFocusRect
        text: qsTr("计算区域:")
        anchors.left: viewWin.right
        anchors.leftMargin: 50
        y:50
        font.pixelSize: 40
    }

    Text{
        id: lfoucsx
        text:"x:"
        anchors.left: lbFocusRect.left
        anchors.leftMargin: 80
        anchors.top: lbFocusRect.bottom
        anchors.topMargin: 10
        font.pixelSize: 40
    }

    TextField{
        id: focusx
        width: 100
        anchors.left: lfoucsx.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lfoucsx.verticalCenter
    }
    Text{
        id: lfoucsy
        text:"y:"
        anchors.left: focusx.right
        anchors.leftMargin: 200
        anchors.verticalCenter: lfoucsx.verticalCenter
        font.pixelSize: 40
    }
    TextField{
        id: focusy
        width: 100
        anchors.left: lfoucsy.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lfoucsx.verticalCenter
    }
    Text{
        id: lfoucswidth
        text:"width:"
        anchors.right: lfoucsx.right
        anchors.top: lfoucsx.bottom
        anchors.topMargin: 20
        font.pixelSize: 40
    }
    TextField{
        id: focuswidth
        width: 100
        anchors.left: lfoucswidth.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lfoucswidth.verticalCenter
    }
    Text{
        id: lfoucsheight
        text:"height:"
        anchors.right: lfoucsy.right
        anchors.verticalCenter: lfoucswidth.verticalCenter
        font.pixelSize: 40
    }
    TextField{
        id: focusheight
        width: 100
        anchors.left: lfoucsheight.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lfoucsheight.verticalCenter
    }
    Button{
        id:foucsrectBt
        anchors.left: focusheight.right
        anchors.leftMargin: 80
        anchors.verticalCenter: lfoucsheight.verticalCenter
        text: qsTr("设置")
        font.pixelSize: 40
        onClicked: {
            ExGlobal.updateCaliParam("LightX",focusx.text)
            ExGlobal.updateCaliParam("LightY",focusy.text)
            ExGlobal.updateCaliParam("LightWidth",focuswidth.text)
            ExGlobal.updateCaliParam("LightHeight",focusheight.text)
        }
    }

    Text{
        id: llight
        text:"荧光亮度"
        anchors.top: focusheight.bottom
        anchors.topMargin: 100
        x:1400
        font.pixelSize: 45
    }

    Text{
        id: lightvalue
        text:"0"
        anchors.top: llight.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: llight.horizontalCenter
        font.pixelSize: 45
    }

    Bt2{
        anchors.top: lightvalue.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: lightvalue.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        textcolor: "#ffffff"
        image: "qrc:/images/confirmbt.png"
        text:qsTr("获取亮度")
        onClicked: {
            doingBox.show(qsTr("获取荧光亮度"))
            Sequence.startLight()
        }
    }

    Bt1 {
        id: btCannel
        onClicked: {
            Sequence.stopView();
            Sequence.actionDo("Light",1,0,0,0);
            mainView.pop();
            Sequence.changeTitle(qsTr("设置"));
        }
    }

    Loading{
        id:doingBox
        anchors.fill: parent
    }

    Component.onCompleted: {
        Sequence.changeTitle(qsTr("荧光亮度测试"))
        focusx.text = ExGlobal.getCaliParam("LightX")
        focusy.text = ExGlobal.getCaliParam("LightY")
        focuswidth.text = ExGlobal.getCaliParam("LightWidth")
        focusheight.text = ExGlobal.getCaliParam("LightHeight")
    }

    Connections{
        target: Sequence
        onCallQmlRefeshView:{
            cameraBox.source = "";
            cameraBox.source = "image://CodeImg/view";
        }
        onSequenceFinish:{
            if (result == Sequence.Result_DoorToWork_ok)
                doingBox.close()
        }
        onRefreshLightValue:{
            doingBox.close()
            lightvalue.text = Sequence.getLLight().toFixed(1)
        }
    }
}
