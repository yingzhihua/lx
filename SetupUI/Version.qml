import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
import "../components"
Item {
    Text {
        id: ver
        font.pixelSize: 40
        y:200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -200
        text: qsTr("主机板版本：")+ExGlobal.version
    }
    Text {
        id: tempver
        anchors.top: ver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("温控板版本：")+ExGlobal.tempversion
    }
    Text {
        id: ctrlver
        anchors.top: tempver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("驱动板版本：")+ExGlobal.ctrlversion
    }
    Text {
        id: ipver
        anchors.top: ctrlver.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("本机IP地址：")+ExGlobal.getIP()
    }
    Text {
        id: fun1
        anchors.top: ipver.bottom
        anchors.topMargin: 100
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("风扇一转速：")+Sequence.fan1Speed
    }
    Text {
        id: fun2
        anchors.top: fun1.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("风扇二转速：")+Sequence.fan2Speed
    }
    Text {
        id: fun3
        anchors.top: fun2.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: ver.horizontalCenter
        font.pixelSize: 40
        text: qsTr("风扇三转速：")+Sequence.fan3Speed
    }

    Text {
        id: updatelog
        anchors.left: ver.right
        anchors.leftMargin: 200
        y: 20
        wrapMode: Text.Wrap
        text:"V2.08\n1、增加测试用时显示。\nV2.09\n1、关闭背光补偿，背光补偿值：0，1，2，缺省值：1\n\
V2.10\n1、增加图像清晰度识别算法，在摄像头设置界面显示，后面用于自动对焦。\n\
V2.11\n1、修改图像分析中心偏移bug。\n2、修改出仓识别错误bug，需要更改配置文件。\n3、增加编码器代码。\n4、改善系统定时器精度。\n\
V2.12\n1、增加自动对焦功能。\n\
"
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

    Component.onCompleted: {
        if (ExGlobal.projectMode() !== 0){
            fun1.visible = false;
            fun2.visible = false;
            fun3.visible = false;
            updatelog.visible = false;
        }
    }
}
