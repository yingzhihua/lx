import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    property var itemList:[]
    Text{
        id: panelName
        text:ExGlobal.panelName+" / "+ExGlobal.panelCode
        font.pixelSize: 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -100
        y:20
    }
    Image {
        id: printIco
        anchors.verticalCenter: panelName.verticalCenter
        anchors.left: panelName.right
        anchors.leftMargin: 10
        source: "qrc:/image/Print.png"
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
        anchors.rightMargin: 20
        Text{
            id:reference
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            text:qsTr("测试有效")
        }
        Image {
            id: validico
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -50
            anchors.top: reference.bottom
            anchors.topMargin: 10
            source: "qrc:/image/Valid.png"
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: +20
            anchors.verticalCenter: validico.verticalCenter
            text:qsTr("参照物")
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                dataView.push("qrc:/DataUI/DataLine.qml");
            }
        }
    }

    Grid{
        id: gridPos
        anchors.top: parent.top
        anchors.topMargin: 200
        anchors.left: divider.right
        anchors.leftMargin: 50
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
                    source: "qrc:/image/Positive.png"
                }
                Text{
                    anchors.verticalCenter: parent.verticalCenter
                    x:50
                    text: ExGlobal.getPosName(itemList[modelData])
                    font.pixelSize: 30
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("click:"+parent.objectName);
                        testResultModel.setCurrItem(parent.objectName);
                        dataView.push("qrc:/DataUI/DataLine.qml");
                    }
                }
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
            dataView.pop();
        }
    }
    Component.onCompleted: {
        itemList = ExGlobal.getBoxItemList("201");
    }
}
