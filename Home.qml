import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Page {
    property string titlemsg: ""
    property bool enableTabBar: true
    property int testprocess: 1000
    id: home_page

    StackView{
        id: stackView
        anchors.fill: parent
        initialItem: ExGlobal.debug?"./HomeUI/Idle.qml":"./HomeUI/Startup.qml"
        //initialItem: "./HomeUI/Idle.qml"
    }
    onTitlemsgChanged: {
        testCompleted.width = 0;
        testunCompleted.width = 0;
        headerMsg.text = titlemsg;
    }
    onEnableTabBarChanged: tabBar.enabled = enableTabBar;

    onTestprocessChanged: {
        console.log("onTestprocessChanged,result:"+testprocess);
        if (testprocess >= 1000)
        {
            testCompleted.width = 0;
            testunCompleted.width = 0;
        }
        else
        {
            testCompleted.width = testprocess*headerRec.width/1000;
            testunCompleted.x = testCompleted.width;
            testunCompleted.width = (1000-testprocess)*headerRec.width/1000;

        }
    }
}
