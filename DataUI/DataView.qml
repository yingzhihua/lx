import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    property var itemList:[]

    Rectangle{
        id:testmsg
        y:159
        width: parent.width
        height: 114
        color: "#DDDDDD"

        Text{
            id: panelName
            text:testModel.getCurrTestPanelName()
            font.pixelSize: 40
            anchors.left: parent.left
            anchors.leftMargin: 500
            anchors.verticalCenter: parent.verticalCenter
        }

        Text{
            id: panelCode
            text:testModel.getCurrTestPanelCode()
            font.pixelSize: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        Text{
            id: boxSerial
            text:testModel.getCurrTestSerial()
            font.pixelSize: 40
            anchors.right: parent.right
            anchors.rightMargin: 500
            anchors.verticalCenter: parent.verticalCenter
        }

        Image {
            id: printIco
            anchors.right: parent.right
            anchors.rightMargin: 200
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/images/Print.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if (Sequence.printTest())
                        busyIndicator.running = true;
                }
            }
        }
    }

    Rectangle{
        id:icmsg
        anchors.top: testmsg.bottom
        anchors.topMargin: 0
        width: parent.width
        height: 114
        color: "#f5f5f5"

        Text{
            id:reference
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 40
            anchors.left: parent.left
            anchors.leftMargin: 700
            color: "#898989"
            text:qsTr("测试有效")
        }

        Rectangle{
            height: parent.height
            width: 200
            anchors.left: reference.right
            anchors.leftMargin: 150
            color: "transparent"

            Image {
                id: validico
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/images/Ref.png"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: validico.right
                anchors.leftMargin: 20
                font.pixelSize: 40
                color: "#898989"
                text:qsTr("参照物")
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    testResultModel.setCurrItem(2);
                    mainView.push("qrc:/DataUI/DataLine.qml");
                }
            }
        }
    }

    Grid{
        id: gridPos
        anchors.top: icmsg.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -150
        columns:4
        rowSpacing: 20
        columnSpacing: 30

        Repeater{
            model: itemList.length
            Rectangle{
                width: 300
                height: 80
                objectName: itemList[modelData]
                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    x:10
                    source: testResultModel.getItemResult(testResultModel.getTestid(),(itemList[modelData]))>0?"qrc:/images/Positive.png":"qrc:/images/Negative.png"
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

    Bt1 {
        id: btCannel
        onClicked: {            
            mainView.pop();
            Sequence.setTitle("datamenu")
            if (ExGlobal.dataEntry() === 1)
                mainView.push("qrc:/HomeUI/Idle.qml");
        }
    }

    Bt2{
        id: btCheck
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btCannel.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        textcolor: "#ffffff"
        image: "qrc:/images/confirmbt.png"
        onClicked: {
            if (btCheck.text === qsTr("撤回审核"))
                testModel.uncheckTest();
            else
                testModel.checkTest();
            mainView.pop();
            Sequence.setTitle("datamenu")
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
            {
                btCheck.textcolor = "#323232"
                btCheck.image = "qrc:/images/CancelTest.png"
                btCheck.text = qsTr("撤回审核")
            }
            else
            {
                btCheck.textcolor = "#ffffff"
                btCheck.image = "qrc:/images/confirmbt.png"
                btCheck.text = qsTr("通过审核")
            }
        }
        else
            btCheck.visible = false;
        Sequence.setTitle("dataview")
        Sequence.changeTitle(testModel.getCurrTestCode() + " | " + testModel.getCurrTestInfo())
        if (testResultModel.getItemResult(testResultModel.getTestid(),2)===0)
        {
            reference.text = qsTr("测试无效")
            reference.color = "#fc7050"
        }

        console.debug(itemList)
    }


    Connections{
        target: Sequence
        onSequenceFinish:{
            if (result === Sequence.Result_Print_finish)
            {
                busyIndicator.running = false;
                promptDialog.show(qsTr("打印完成"))
            }
            else if(result === Sequence.Result_Print_Error){
                busyIndicator.running = false;
                promptDialog.show(qsTr("未找到打印机"))
            }
        }
    }
}
