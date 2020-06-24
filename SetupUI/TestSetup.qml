import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import Dx.Sequence 1.0
import Dx.Global 1.0
import "../components"
Item {
    Label{
        id:selectlabel
        height: 20
        anchors.left: parent.left
        anchors.leftMargin: 50
        anchors.top: parent.top
        anchors.topMargin: 30
        text: qsTr("时序:")        
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment :Text.AlignVCenter
    }
    ComboBox {
        id :panelcombo
        width: 300
        anchors.left: selectlabel.right
        anchors.leftMargin: 10
        anchors.verticalCenter: selectlabel.verticalCenter
        currentIndex: 0
        model: Sequence.TestList;
    }
    Button {
        id: btSet          
        text: qsTr("设置")
        anchors.left: panelcombo.right
        anchors.leftMargin: 20
        anchors.verticalCenter: selectlabel.verticalCenter
        onClicked: {
            Sequence.sequenceSetPanel(panelcombo.currentText);
        }
    }
    Button {
        id: btLoad
        text: qsTr("加载配置文件")
        anchors.left: btSet.right
        anchors.leftMargin: 100
        anchors.verticalCenter: selectlabel.verticalCenter
        onClicked: {
            Sequence.sequenceInit();
            //Sequence.openErr("lengxing");
            //window.waitMsg = "lengxing"
        }
    }
    Button {
        id: btTest
        anchors.left: btLoad.right
        anchors.leftMargin: 100
        anchors.verticalCenter: btLoad.verticalCenter
        text: qsTr("测试")
        onClicked: {
            Sequence.lxDebug();
        }
    }

    CheckBox{
        id:cbChildImage
        anchors.left: btTest.right
        anchors.leftMargin: 100
        anchors.verticalCenter: btLoad.verticalCenter
        text: "记录子图像"
        checked: ExGlobal.childImage
        onClicked: {
            ExGlobal.childImage = cbChildImage.checked;
        }
    }

    CheckBox{
        id:cbAutoFocus
        anchors.left: cbChildImage.right
        anchors.leftMargin: 50
        anchors.verticalCenter: btLoad.verticalCenter
        text: qsTr("打开自动对焦")
        checked: ExGlobal.autoFocus == 1?true:false
        onClicked: {
            if (cbAutoFocus.checked)
                ExGlobal.autoFocus = 1;
            else
                ExGlobal.autoFocus = 0;
        }
    }

    CheckBox{
        id:cbqrCode
        anchors.left: cbAutoFocus.right
        anchors.leftMargin: 50
        anchors.verticalCenter: btLoad.verticalCenter
        text: qsTr("打开二维码识别")
        checked: ExGlobal.qrCode == 1?true:false
        onClicked: {
            if (cbqrCode.checked)
                ExGlobal.qrCode = 1;
            else
                ExGlobal.qrCode = 0;
        }
    }

    CheckBox{
        id: openlight
        anchors.left: parent.left
        anchors.leftMargin: 50
        anchors.top: btTest.bottom
        anchors.topMargin: 100
        checked: true
        text: qsTr("开灯")
    }

    CheckBox{
        id: handleimage
        anchors.left: openlight.right
        anchors.leftMargin: 30
        anchors.verticalCenter: openlight.verticalCenter
        checked: false
        text: qsTr("识别前处理图像")
    }

    CheckBox{
        id: scaleimage
        anchors.left: handleimage.right
        anchors.leftMargin: 30
        anchors.verticalCenter: openlight.verticalCenter
        checked: false
        text: qsTr("剪切图像")
    }

    Label{
        id:l1
        height: 20
        anchors.left: scaleimage.right
        anchors.leftMargin: 50
        anchors.verticalCenter: openlight.verticalCenter
        text: qsTr("二值化阀值：")
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment :Text.AlignVCenter
    }

    SpinBox{
        id:binF
        from:10
        value: 120
        to:200
        width: 200
        anchors.left: l1.right
        anchors.leftMargin: 10
        anchors.verticalCenter: openlight.verticalCenter
    }

    Label{
        id:l2
        height: 20
        anchors.left: binF.right
        anchors.leftMargin: 50
        anchors.verticalCenter: openlight.verticalCenter
        text: qsTr("曝光时间：")
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment :Text.AlignVCenter
    }

    SpinBox{
        id:pox
        from:10
        value: 1000
        to:3000
        width: 200
        stepSize: 10
        anchors.left: l2.right
        anchors.leftMargin: 10
        anchors.verticalCenter: openlight.verticalCenter
    }

    Button {
        id: btQr
        text: qsTr("识别二维码")
        anchors.left: parent.left
        anchors.leftMargin: 50
        anchors.top: openlight.bottom
        anchors.topMargin: 30
        width: 300
        height: 80
        onClicked: {
            qrBox.source = "";
            qrBox2.source = "";
            messageTest.text = "";
            l3.visible = false;
            l4.visible = false;
            Sequence.qrSet(openlight.checked,scaleimage.checked,handleimage.checked,binF.value,pox.value);
            Sequence.sequenceDo(Sequence.Sequence_QrDecode);
        }
    }

    Label{
        id:messageTest
        height: 20
        anchors.left: btQr.right
        anchors.leftMargin: 30
        anchors.verticalCenter: btQr.verticalCenter
        text: qsTr("Message")
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment :Text.AlignVCenter
    }

    Image {
        id: qrBox
        anchors.left: parent.left
        anchors.leftMargin: 50
        anchors.top: messageTest.bottom
        anchors.topMargin: 20
        width: 440
        height: 440        
        cache: false
        source: ""
    }
    Image {
        id: qrBox2
        anchors.left: qrBox.right
        anchors.leftMargin: 10
        anchors.top: messageTest.bottom
        anchors.topMargin: 20
        width: 300
        height: 440
        fillMode: Image.Pad
        cache: false
        source: ""
    }
    Image {
        id: qrBox3
        anchors.left: qrBox2.right
        anchors.leftMargin: 10
        anchors.top: messageTest.bottom
        anchors.topMargin: 20
        width: 300
        height: 440
        fillMode: Image.Pad
        cache: false
        source: ""
    }
    Label{
        id:l3
        height: 20
        anchors.top: qrBox.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: qrBox.horizontalCenter
        text: qsTr("原图")
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment :Text.AlignVCenter
        visible: false
    }
    Label{
        id:l4
        height: 20
        anchors.top: qrBox2.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: qrBox2.horizontalCenter
        text: qsTr("处理图")
        horizontalAlignment : Text.AlignHCenter
        verticalAlignment :Text.AlignVCenter
        visible: false

    }

    GroupBox{
        id:qrxy
        title: qsTr("二维码剪切坐标")
        anchors.left: qrBox3.right
        anchors.leftMargin: 50
        anchors.top: messageTest.bottom
        anchors.topMargin: 20
        width: 850
        font.pixelSize: 40

        Grid{
            id: gridqr
            //anchors.horizontalCenter: parent.horizontalCenter
            columns:5
            rowSpacing: 20
            columnSpacing: 30

            Text{
                text:qsTr("左上角坐标")
            }
            Text{
                text:qsTr("X：")
            }
            SpinBox{
                id:x1
                from:0
                value: ExGlobal.getCaliParam("QrX1")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrX1",x1.value)
            }
            Text{
                text:qsTr("Y：")
            }
            SpinBox{
                id:y1
                from:0
                value: ExGlobal.getCaliParam("QrY1")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrY1",y1.value)
            }

            Text{
                text:qsTr("右上角坐标")
            }
            Text{
                text:qsTr("X：")
            }
            SpinBox{
                id:x2
                from:0
                value: ExGlobal.getCaliParam("QrX2")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrX2",x2.value)
            }
            Text{
                text:qsTr("Y：")
            }
            SpinBox{
                id:y2
                from:0
                value: ExGlobal.getCaliParam("QrY2")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrY2",y2.value)
            }

            Text{
                text:qsTr("右下角坐标")
            }
            Text{
                text:qsTr("X：")
            }
            SpinBox{
                id:x3
                from:0
                value: ExGlobal.getCaliParam("QrX3")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrX3",x3.value)
            }
            Text{
                text:qsTr("Y：")
            }
            SpinBox{
                id:y3
                from:0
                value: ExGlobal.getCaliParam("QrY3")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrY3",y3.value)
            }

            Text{
                text:qsTr("左下角坐标")
            }
            Text{
                text:qsTr("X：")
            }
            SpinBox{
                id:x4
                from:0
                value: ExGlobal.getCaliParam("QrX4")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrX4",x4.value)
            }
            Text{
                text:qsTr("Y：")
            }
            SpinBox{
                id:y4
                from:0
                value: ExGlobal.getCaliParam("QrY4")
                to:2000
                stepSize: 1
                font.pixelSize: 40
                width: 220
                onValueChanged: ExGlobal.updateCaliParam("QrY4",y4.value)
            }
        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            mainView.pop();
        }
    }

    Button {
        id: btquit
        text: qsTr("关闭程序")
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 50
        onClicked: {
            Qt.quit()
        }
    }

    MessageDialog{
        id:messageDialog
        title: "Please choose an image file"
        function show(caption){
            messageDialog.text = caption;
            messageDialog.open();
        }

    }

    Connections{
        target: Sequence
        onCallQmlRefeshData:{
            console.log(data.length,data);
        }
        onQrDecode:{
            console.log(info.length,info);
            messageTest.text = info;
        }
        onCallQmlRefeshQrImg:{
            //qrBox.source = "";
            qrBox.source = "image://CodeImg/qr";
            qrBox2.source = "image://CodeImg/qr2";
            qrBox3.source = "image://CodeImg/qr3";
            l3.visible = true;
            l4.visible = true;
            console.log("TestSetup.qml,onCallQmlRefeshQrImg");
        }
    }

}
