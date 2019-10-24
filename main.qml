import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Window 2.12
import QtQuick.VirtualKeyboard 2.4

import Dx.Global 1.0
import Dx.Sequence 1.0

ApplicationWindow {
    property string waitMsg: ""
    id: window
    visible: true
    width: screen.width
    height: screen.height
    title: qsTr("闪量-华山")

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: window.height
        width: window.width

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    menuBar: Rectangle{
        id:headerRec
        width: parent.width
        height: 80
        color: "darkslateblue"
        Rectangle{
            id:testCompleted
            x:0
            y:0
            height: parent.height
            width:0
            color:"darkturquoise"
        }
        Rectangle{
            id:testunCompleted
            x:parent.width*0.5
            y:0
            height: parent.height
            width:0
            color:"gainsboro"
        }
        Text {
            id: headerMsg
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: parent.height*0.5
            color: "white"
            text: qsTr("text")
        }
        Text {
            id: headerTitle
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: parent.height*0.5
            color: "white"
            text: ExGlobal.sysName
        }
    }

    header:Rectangle{
        id:headerdatetime
        height: 40
        width: parent.width
        color:"gainsboro"
        Text{
            id:headertime
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.bottom: parent.bottom
            font.pixelSize: 30
            text:qsTr("")
        }
        Text {
            id: headerdate
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            font.pixelSize: 30
            text: qsTr("")
        }
    }

    footer: TabBar{
        id:tabBar
        height:80
        width:parent.width
        currentIndex: 1

        background: Image{
            anchors.fill: parent
            smooth: true
            source:"qrc:/image/footerbk.jpg"
        }

        ListModel{
            id:footerModel
            ListElement{modelText:"设置";modelImgP:"qrc:";modelImg:"qrc";}
            ListElement{modelText:"主页";modelImgP:"qrc:";modelImg:"qrc";}
            ListElement{modelText:"数据";modelImgP:"qrc:";modelImg:"qrc";}
            //ListElement{modelText:"串口";modelImgP:"qrc:";modelImg:"qrc";}
        }
        Repeater{
            model:footerModel
            TabButton{
                height: tabBar.height
                contentItem: Text{
                    text:modelText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: tabBar.height*0.6
                    color:(model.index===tabBar.currentIndex)?"#148014":"#000000"
                }
                background: Rectangle{
                    height: 0
                }
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Setup {
            id:setupPage
        }

        Home {
            id:homePage
        }

        Data {
            id:dataPage
        }

        onCurrentIndexChanged: {
            if (currentIndex == 0)
                headerMsg.text = setupPage.titlemsg
            else if(currentIndex == 1)
                headerMsg.text = homePage.titlemsg
            else if(currentIndex == 2)
                headerMsg.text = dataPage.titlemsg
        }
    }

    ErrPage{
        id:err
        visible: false
    }

    Timer{
        repeat: true
        interval: 1000
        running: true
        onTriggered: {
            headertime.text = Qt.formatTime(new Date(),"hh:mm:ss")
            headerdate.text = Qt.formatDate(new Date(),"yyyy-MM-dd")
        }
    }

    Connections{
        target: Sequence
        onErrOccur:{
            console.log("main.qml,result:"+error);
            if (ExGlobal.debug == false){
                err.errText = error;
                err.visible = true;
                tabBar.enabled = false;
            }
        }
    }

    Component.onCompleted: {
        //console.log("lx-model:"+testModel.rowCount());
    }

    onWaitMsgChanged: {
        if (waitMsg == "")
        {
            //busyIndicator.running = false;
            //waitText.visible = false;
        }
        else
        {
            //waitText.text = waitMsg;
            //waitText.visible = true;
            //busyIndicator.running = false;
            console.log("main.qml,result:"+waitMsg);
        }
    }
}
