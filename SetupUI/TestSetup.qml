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
            //messageDialog.show("lengxing");
        }
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
    }

}
