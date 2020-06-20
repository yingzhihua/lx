import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    property var itemList:[]
    Text{
        id: panelName
        //text: "panelName"
        text:ExGlobal.panelName+" / "+ExGlobal.boxSerial()
        font.pixelSize: 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -100
        y:100
    }
    Image {
        id: printIco
        anchors.verticalCenter: panelName.verticalCenter
        anchors.left: panelName.right
        anchors.leftMargin: 10
        source: "qrc:/image/Print.png"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if (Sequence.printTest())
                    busyIndicator.running = true;
            }
        }
    }

    Image {
        id: divider
        anchors.verticalCenter: parent.verticalCenter
        x: parent.width/3
        source: "qrc:/image/line.png"
    }
    Rectangle{
        width:200
        height:100
        anchors.verticalCenter: divider.verticalCenter
        anchors.right: divider.left
        anchors.rightMargin: 50

        Text{
            id:reference
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            font.pixelSize: 40
            //text:qsTr("测试有效")
            text:qsTr("Internal")
        }

        Image {
            id: validico
            anchors.horizontalCenter: parent.horizontalCenter
            //anchors.horizontalCenterOffset: -60
            anchors.horizontalCenterOffset: -150
            //anchors.top: reference.bottom
            anchors.verticalCenter: reference.verticalCenter
            anchors.verticalCenterOffset: 30
            width: 60
            height: 60
            source: "qrc:/image/Valid4.png"
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            //anchors.horizontalCenterOffset: +20
            //anchors.verticalCenter: validico.verticalCenter
            anchors.top: reference.bottom
            font.pixelSize: 40
            //text:qsTr("参照物")
            text:qsTr("Control")
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                testResultModel.setCurrItem(2);
                mainView.push("qrc:/DataUI/DataLine.qml");
            }
        }
    }

    //*
    Grid{
        id: gridPos
        //anchors.top: parent.top
        //anchors.topMargin: 200
        anchors.verticalCenter: divider.verticalCenter
        anchors.left: divider.right
        anchors.leftMargin: 100
        columns:3
        rowSpacing: 20
        columnSpacing: 30

        Repeater{
            model: itemList.length
            Rectangle{
                width: 200
                height: 80
                objectName: itemList[modelData]
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    x:10
                    source: ExGlobal.getItemResult(testResultModel.getTestid(),(itemList[modelData])>0||modelData==2)?"qrc:/image/Positive.png":"qrc:/image/Negative.png"
                }
                Text{
                    anchors.verticalCenter: parent.verticalCenter
                    x:60
                    text: ExGlobal.getPosName(itemList[modelData])
                    font.pixelSize: 40
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("click:"+parent.objectName);
                        testResultModel.setCurrItem(parent.objectName);                        
                        mainView.push("qrc:/DataUI/DataLine.qml");
                    }
                }
            }
        }
    }
    //*/

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {            
            mainView.pop();
            if (ExGlobal.dataEntry() === 1)
                mainView.push("qrc:/HomeUI/Idle.qml");
        }
    }

    Button {
        id: btCheck
        font.pixelSize: 30
        width: 200
        text: qsTr("审核")
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 80
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            if (btCheck.text === qsTr("撤回审核"))
                testModel.uncheckTest();
            else
                testModel.checkTest();
            mainView.pop();
            if (ExGlobal.dataEntry() === 1)
                mainView.push("qrc:/HomeUI/Idle.qml");
        }
    }

    BusyIndicator{
        id:busyIndicator
        anchors.centerIn: parent
        implicitWidth: 200
        implicitHeight: 200
        opacity: 1.0
        running: false
    }

    AutoCloseMsg{
        id: promptDialog
        anchors.fill: parent
    }

    Component.onCompleted: {
        itemList = ExGlobal.getBoxItemList();
        if (testModel.mayCheck())
        {
            btCheck.visible = true;
            if (testModel.haveCheck())
                btCheck.text = qsTr("撤回审核")
            else
                btCheck.text = qsTr("通过审核")
        }
        else
            btCheck.visible = false;
        console.debug(itemList)
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            if (result == Sequence.Result_Print_finish)
            {
                busyIndicator.running = false;
                promptDialog.show(qsTr("打印完成"))
            }
            else if(result == Sequence.Result_Print_Error){
                busyIndicator.running = false;
                promptDialog.show(qsTr("未找到打印机"))
            }
        }
    }
}
