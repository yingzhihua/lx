import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    property bool bView: false
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

    Text{
        id: definition
        anchors.bottom: viewWin.bottom
        anchors.left: viewWin.right
        anchors.leftMargin: 20
        width: 500
        height: 200
        wrapMode: Text.Wrap
        verticalAlignment: Text.AlignBottom
    }

    Button{
        id:startView
        anchors.left: viewWin.left
        anchors.leftMargin: 10
        anchors.top: viewWin.bottom
        anchors.topMargin: 30
        text: qsTr("打开预览")
        font.pixelSize: 30
        onClicked: {
            viewText.text = qsTr("图像加载中。。。");
            bView = true;
            Sequence.startView(0);
        }
    }

    Button{
        id:stopView
        anchors.left: startView.right
        anchors.leftMargin: 50
        anchors.verticalCenter: startView.verticalCenter
        text: qsTr("停止预览")
        font.pixelSize: 30
        onClicked: {
            bView = false;
            Sequence.stopView();
            viewText.text = "";
            cameraBox.source = "";
        }
    }
    Button{
        id:openLight
        anchors.left: stopView.right
        anchors.leftMargin: 50
        anchors.verticalCenter: startView.verticalCenter
        text: qsTr("打开灯光")
        font.pixelSize: 30
        onClicked: Sequence.actionDo("Light",5,0,0,0)
    }

    Button{
        id:closeLight
        anchors.left: openLight.right
        anchors.leftMargin: 50
        anchors.verticalCenter: startView.verticalCenter
        text: qsTr("关闭灯光")
        font.pixelSize: 30
        onClicked: Sequence.actionDo("Light",1,0,0,0)
    }

    Button{
        id:capture
        anchors.left: closeLight.right
        anchors.leftMargin: 50
        anchors.verticalCenter: startView.verticalCenter
        text: qsTr("拍照")
        font.pixelSize: 30
        onClicked: Sequence.saveView()
    }

    Button{
        id:btfocus
        anchors.left: capture.right
        anchors.leftMargin: 50
        anchors.verticalCenter: startView.verticalCenter
        text: qsTr("自动对焦")
        font.pixelSize: 30
        onClicked: {
            bView = true
            Sequence.autoFocus()
        }
    }

    Text {
        id: lbabs
        text: qsTr("曝光时间:")
        anchors.left: viewWin.right
        anchors.leftMargin: 60
        y:100
        font.pixelSize: 40
    }

    SpinBox{
        id:absValue
        anchors.left: lbabs.right
        anchors.leftMargin: 30
        anchors.verticalCenter: lbabs.verticalCenter
        from:1
        value: Sequence.getAbs()
        to:5000
        stepSize: 10
        width: 250
        transformOrigin: Item.Center
        font.pixelSize: 50
    }

    Button{
        id:absBt
        anchors.left: absValue.right
        anchors.leftMargin: 30
        anchors.verticalCenter: lbabs.verticalCenter
        text: qsTr("设置")
        font.pixelSize: 50
        onClicked: Sequence.setAbs(absValue.value)
    }
    Text {
        id: lbgain
        text: qsTr("增益:")
        anchors.right: lbabs.right
        anchors.top: lbabs.bottom
        anchors.topMargin: 40
        font.pixelSize: 40
    }

    SpinBox{
        id:gainValue
        anchors.left: lbgain.right
        anchors.leftMargin: 30
        anchors.verticalCenter: lbgain.verticalCenter
        from:0
        value: Sequence.getGain()
        to:100
        stepSize: 10
        width: 250
        transformOrigin: Item.Center
        font.pixelSize: 50
    }

    Button{
        id:gainBt
        anchors.left: gainValue.right
        anchors.leftMargin: 30
        anchors.verticalCenter: gainValue.verticalCenter
        text: qsTr("设置")
        font.pixelSize: 50
        onClicked: Sequence.setGain(gainValue.value)
    }

    Text {
        id: lbfocus
        text: qsTr("焦距:")
        anchors.right: lbgain.right
        anchors.top: lbgain.bottom
        anchors.topMargin: 40
        font.pixelSize: 40
    }

    SpinBox{
        id:focusValue
        anchors.left: lbfocus.right
        anchors.leftMargin: 30
        anchors.verticalCenter: lbfocus.verticalCenter
        from:0
        value: ExGlobal.getCaliParam("CamFocus")
        to:4400
        stepSize: 50
        width: 250
        transformOrigin: Item.Center
        font.pixelSize: 50
    }

    Button{
        id:focusBt
        anchors.left: focusValue.right
        anchors.leftMargin: 30
        anchors.verticalCenter: focusValue.verticalCenter
        text: qsTr("设置")
        font.pixelSize: 50
        onClicked: {
            if (Sequence.actionDo("Focus",2,focusValue.value+2600,0,0))
                ExGlobal.updateCaliParam("CamFocus",focusValue.value);
        }

    }

    Button{
        id:focusResetBt
        anchors.left: focusBt.right
        anchors.leftMargin: 30
        anchors.verticalCenter: focusValue.verticalCenter
        text: qsTr("复位")
        font.pixelSize: 50
        onClicked: {
            if (Sequence.actionDo("Focus",1,0,0,0))
            {
                focusValue.value = 0;
                ExGlobal.updateCaliParam("CamFocus",focusValue.value);
            }
        }
    }

    CheckBox {
        id: lbwhite
        text: qsTr("打开自动白平衡")
        anchors.right: focusValue.right
        anchors.top: lbfocus.bottom
        checked: Sequence.getWhiteBalance() === 0
        anchors.topMargin: 80
        font.pixelSize: 40
    }
    Text {
        id: lbwhiteTemperture
        text: qsTr("白平衡温度:")
        anchors.right: lbgain.right
        anchors.top: lbwhite.bottom
        anchors.topMargin: 40
        font.pixelSize: 40
    }
    SpinBox{
        id:whiteValue
        anchors.left: lbwhiteTemperture.right
        anchors.leftMargin: 30
        anchors.verticalCenter: lbwhiteTemperture.verticalCenter
        from:2800
        value: Sequence.getWhiteBalance()
        to:6500
        stepSize: 100
        width: 250
        transformOrigin: Item.Center
        font.pixelSize: 50
    }

    Button{
        id:whiteBt
        anchors.left: whiteValue.right
        anchors.leftMargin: 30
        anchors.verticalCenter: whiteValue.verticalCenter
        text: qsTr("设置")
        font.pixelSize: 50
        onClicked: Sequence.setWhiteBalance(lbwhite.checked?0:whiteValue.value)
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

    Component.onCompleted: {
        Sequence.changeTitle(qsTr("摄像头测试"))
    }

    Connections{
        target: Sequence
        onCallQmlRefeshView:{
            if (bView == true){
                cameraBox.source = "";
                cameraBox.source = "image://CodeImg/view";
                definition.text = "清晰度："+Sequence.getDefinition1().toFixed(4)+",  "+Sequence.getDefinition2().toFixed(4)+",  "+Sequence.getDefinition3().toFixed(4)
                definition.text += "\n亮度："+Sequence.getLight().toFixed(4)
                definition.text += "\n圆直径："+Sequence.getCircSize().toFixed(4)
                console.log("CameraSetup.qml,onCallQmlRefeshImg");
            }
        }

        onAutoFocusNotify:{
            focusValue.value = value;
        }
    }
}
