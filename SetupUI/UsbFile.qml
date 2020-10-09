import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import "../components"

Rectangle {
    property int action: 0
    Rectangle{
        id: footer
        x:0
        anchors.bottom: parent.bottom
        height: 150
        width: parent.width
        color: "#e6e6e6"
        Row{
            anchors.centerIn: parent
            spacing: 100
            Bt2{
                width: 250
                height: 85
                image: "qrc:/images/cancel.png"
                text: qsTr("删除")
                textcolor: "#464646"
                textoffsetx: 30
                textoffsety: -3
                onClicked: usbModel.deleteDir()
            }

            Bt2{
                width: 250
                height: 85
                image: "qrc:/images/confirm.png"
                text: qsTr("拷出")
                textcolor: "#ffffff"
                textoffsetx: 30
                textoffsety: -3
                onClicked: {
                    msg.show(qsTr("文件拷贝中"))
                    Sequence.updateFooter(false,false,false)
                    usbModel.copyDir()                    
                }
            }

            Bt2{
                width: 250
                height: 85
                image: "qrc:/images/confirm.png"
                text: qsTr("升级")
                textcolor: "#ffffff"
                textoffsetx: 30
                textoffsety: -3
                onClicked: {
                    usbModel.updateSoft()
                    promptClose.show(qsTr("升级完成！"))
                }
            }

            Bt2{
                width: 250
                height: 85
                image: "qrc:/images/confirm.png"
                text: qsTr("重启")
                textcolor: "#ffffff"
                textoffsetx: 30
                textoffsety: -3
                onClicked: {                    
                    usbModel.restart()
                }
            }

            Bt2{
                width: 250
                height: 85
                image: "qrc:/images/backTestbt.png"
                text: qsTr("返回")
                textcolor: "#464646"
                textoffsetx: 30
                textoffsety: -3
                onClicked: mainView.pop()
            }
        }
    }

    ScrollView{
        x:0
        y:0
        width: parent.width
        height: parent.height - footer.height
        ScrollBar.vertical.width: 50
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        clip:true

        Grid{
            width: parent.width - 50
            height: parent.height
            columns:4
            rowSpacing: 20
            columnSpacing: 10

            Repeater{
                model: usbModel
                Rectangle{
                    width: 450
                    height: 100
                    color: Select?"#f5f5f5":"#ffffff"
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        x:10
                        source: IsDir?"qrc:/images/dir.png":"qrc:/images/file.png"
                    }

                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        x:60
                        height: parent.height
                        width: parent.width-100
                        wrapMode: Text.Wrap
                        clip:true
                        text: Name
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 40
                    }

                    CheckBox{
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        checked: Select
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            console.log(index)
                            usbModel.switchSelect(index)
                        }
                    }
                }
            }
        }
    }

    Doing{
        id:msg
        anchors.fill: parent
        onCloseAck: {
            usbModel.stopCopy()
        }
    }

    OneBtnQuery{
        id:promptClose
        anchors.fill: parent
        bttext: qsTr("返回")
        onCloseAck: {
        }
    }

    Component.onCompleted: {
        usbModel.refresh()
    }

    Connections{
        target: usbModel
        onCopyFinish:{
            msg.close()
            Sequence.updateFooter(true,true,true)
        }
    }
}
