import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
import Dx.Sequence 1.0
Item {    
    id:lang_menu
    ListModel{
        id:languageModel
        ListElement{name:"中文"}
        ListElement{name:"English"}
    }

    Column{
        Repeater{
            model: languageModel

            Rectangle{
                height: 120
                width: lang_menu.width
                color: (model.index === ExGlobal.sysLanguageCode)?"#dedede":"#ffffff"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 175
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 35
                    text: name
                }

                Image {
                    id:row
                    anchors.right: parent.right
                    anchors.rightMargin: 120
                    visible: (model.index === ExGlobal.sysLanguageCode)?true:false
                    width: 39
                    height: 33
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.Pad
                    source: "qrc:/images/check.png"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        ExGlobal.sysLanguageCode = model.index;
                        console.log(model.index,ExGlobal.sysLanguageCode)
                        mainView.pop();
                    }
                }
            }
        }
    }
/*
    RadioButton{
        id:rbEnglish
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100
        text:"English"
        font.pixelSize: 60
        onClicked: {
            console.log("select english")
            ExGlobal.sysLanguageCode = 0;
        }
    }

    RadioButton{
        id:rbChinese
        anchors.top: rbEnglish.bottom
        anchors.topMargin: 100
        anchors.left: rbEnglish.left
        anchors.leftMargin: 0
        font.pixelSize: 60
        text:"中文"
        onClicked: {
            console.log("select china")
            ExGlobal.sysLanguageCode = 1;
        }
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
*/
    Component.onCompleted: {
        Sequence.setTitle("setup_language")
    }
}
