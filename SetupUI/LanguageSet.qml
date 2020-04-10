import QtQuick 2.12
import QtQuick.Controls 2.5
import "../components"
import Dx.Global 1.0
Item {    

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

    Component.onCompleted: {
        if(ExGlobal.sysLanguageCode == 0)
            rbEnglish.checked = true;
        else if(ExGlobal.sysLanguageCode == 1)
            rbChinese.checked = true;
    }
}
