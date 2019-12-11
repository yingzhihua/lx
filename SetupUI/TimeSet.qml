import QtQuick 2.12
import QtQuick.Controls 1.6
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import "../components"
import Dx.Global 1.0

Item {
    Calendar{
        id: calendar
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -100        

        style: CalendarStyle{
            gridVisible: false
        }

        onSelectedDateChanged: {
            console.debug(calendar.selectedDate.toLocaleDateString(Qt.locale("en_US"),"yyyy-MM-dd"))
            console.debug("2----",calendar.selectedDate.toLocaleString(Qt.locale("en_US"),"yyyy-MM-dd HH:mm:ss"))
            console.debug(calendar.selectedDate.getTime())
        }
        onClicked: {
            console.debug("click",date)
        }
    }

    Row{
        id:rowData
        anchors.top: calendar.bottom
        anchors.topMargin: 50
        anchors.left: calendar.left
        anchors.leftMargin: -100

        Text {
            width: 90
            horizontalAlignment: Text.AlignRight
            text: qsTr("时：")
        }

        SpinBox{
            id:spHour
            width: 150
            from:0
            to:23
            stepSize: 1
        }
        Text{
            width: 90
            horizontalAlignment: Text.AlignRight
            text:qsTr("分：")
        }
        SpinBox{
            id:spMinute
            width: 150
            from:0
            to:59
        }
        Text{
            width: 90
            horizontalAlignment: Text.AlignRight
            text:qsTr("秒：")
        }
        SpinBox{
            id:spSecond
            width: 150
            from:0
            to:59
        }
    }

    Button{
        id:startView
        anchors.horizontalCenter: calendar.horizontalCenter
        anchors.top: rowData.bottom
        anchors.topMargin: 50
        text: qsTr("设置")

        onClicked: {
            var setStr = calendar.selectedDate.toLocaleDateString(Qt.locale("en_US"),"yyyy-MM-dd")+" "+spHour.value+":"+spMinute.value+":"+spSecond.value
            console.debug("set--",setStr)
            ExGlobal.setTime(setStr)
        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            setupView.pop();
        }
    }
    Component.onCompleted: {
        console.debug(calendar.selectedDate.toLocaleDateString())
        if (ExGlobal.sysLanguageCode == 0)
            calendar.locale = Qt.locale("en_US");

        var dt = new Date();
        spHour.value = dt.getHours()
        spMinute.value = dt.getMinutes()
        spSecond.value = dt.getSeconds()
    }
}
