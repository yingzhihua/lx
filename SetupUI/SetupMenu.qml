import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
Rectangle {
    property string titlemsg: qsTr("设置")
    id: setup_page
    color: "darkgrey"
    ListModel{
        id:setModel
        ListElement{name:qsTr("系统名称");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{name:qsTr("语言");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{name:qsTr("管理员密码");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{name:qsTr("网络");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{name:qsTr("时间设置");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{name:qsTr("用户帐号");info:"";qrc_res:"qrc:/SetupUI/Incomplete.qml"}
        ListElement{name:qsTr("机械校准");info:"";qrc_res:"qrc:/SetupUI/MachineCalibration.qml"}
        //ListElement{name:qsTr("温度控制");info:"";qrc_res:"qrc:/SetupUI/TempCtrl.qml"}
        //ListElement{name:qsTr("帕尔贴校准");info:"";qrc_res:"qrc:/SetupUI/TempCali.qml"}
        ListElement{name:qsTr("摄像头参数设置");info:"";qrc_res:"qrc:/SetupUI/CameraSetup.qml"}
        ListElement{name:qsTr("测试设置");info:"";qrc_res:"qrc:/SetupUI/TestSetup.qml"}
        ListElement{name:qsTr("版本号");info:"";qrc_res:"qrc:/SetupUI/Version.qml"}
    }

    ScrollView{
        anchors.fill: parent

        Column{
            Repeater{
                model: setModel

                Rectangle{
                    height: 80
                    width: dataPage.width
                    //color: (model.index%2 == 1)?"#ffffff":"lightgrey"
                    color: "darkseagreen"
                    Rectangle{
                        height: 1
                        color: "white"
                        anchors.top: parent.top
                        width:parent.width
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 30
                        text: name
                    }
                    Text {
                        x:parent.width*0.4
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 30
                        text: info
                    }
                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 30
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 30
                        text: ">"
                    }
                    Rectangle{
                        height: 1
                        color: "black"
                        anchors.bottom: parent.bottom
                        width:parent.width
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if (qrc_res != "")
                                setupView.push(qrc_res);
                        }
                    }
                }
            }

        }
    }

    onTitlemsgChanged: headerMsg.text = titlemsg;

    Component.onCompleted: {
        setModel.get(0).info = ExGlobal.sysName;
        setModel.get(1).info = ExGlobal.sysLanguageName;
    }
}
