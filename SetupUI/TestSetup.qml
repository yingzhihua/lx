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
        id: openlight
        anchors.left: parent.left
        anchors.leftMargin: 50
        anchors.top: btTest.bottom
        anchors.topMargin: 50
        checked: true
        text: qsTr("开灯")
    }

    CheckBox{
        id: handleimage
        anchors.left: openlight.right
        anchors.leftMargin: 30
        anchors.verticalCenter: openlight.verticalCenter
        checked: true
        text: qsTr("识别前处理图像")
    }

    CheckBox{
        id: scaleimage
        anchors.left: handleimage.right
        anchors.leftMargin: 30
        anchors.verticalCenter: openlight.verticalCenter
        checked: true
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
        value: 200
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
        anchors.leftMargin: 50
        anchors.top: messageTest.bottom
        anchors.topMargin: 20
        width: 440
        height: 440
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
    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            setupView.pop();
        }
    }

    Button {
        id: btquit
        text: qsTr("关闭程序")
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 100
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
            qrBox.source = "";
            qrBox.source = "image://CodeImg/qr";
            qrBox2.source = "image://CodeImg/qr2";
            l3.visible = true;
            l4.visible = true;
            console.log("TestSetup.qml,onCallQmlRefeshQrImg");
        }
    }

}
