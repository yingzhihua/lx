import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
Page {
    property int titleChange: 0
    id: start_page
    Image {
        id: appicon
        anchors.centerIn: parent

        source: "qrc:/image/Dxlogo.png"
    }
    Component.onCompleted: {
        home_page.titlemsg=qsTr("系统启动......");
        home_page.enableTabBar = false;
        Sequence.sequenceInit();
        Sequence.sequenceDo(Sequence.Sequence_SelfCheck);
    }

    Connections{
        target: Sequence
        onSequenceFinish:{
            if (result == Sequence.Result_SelfCheck_ok)
            {
                changeTimer.stop();
                stackView.pop();
                stackView.push("qrc:/HomeUI/Login.qml");
            }
            console.log("Startup.qml,result:"+result);
        }
    }

    Timer{
        id: changeTimer
        repeat: true
        interval: 500
        running: true
        onTriggered: {
            if (titleChange == 0)
                home_page.titlemsg=qsTr("系统启动");
            else if(titleChange == 1)
                home_page.titlemsg=qsTr("系统启动.");
            else if(titleChange == 2)
                home_page.titlemsg=qsTr("系统启动..");
            else if(titleChange == 3)
                home_page.titlemsg=qsTr("系统启动...");
            else if(titleChange == 4)
                home_page.titlemsg=qsTr("系统启动....");
            else if(titleChange == 5)
                home_page.titlemsg=qsTr("系统启动.....");
            else
                home_page.titlemsg=qsTr("系统启动......");
            titleChange++;
            if (titleChange>6)
                titleChange = 0;

        }
    }
}
