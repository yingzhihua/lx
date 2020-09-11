import QtQuick 2.12
import QtQuick.Controls 1.6
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import "../components"
import Dx.Global 1.0
import Dx.Sequence 1.0
Rectangle {
    id:rect
    color: "#f5f5f5"
    Calendar{
        id: calendar        
        width: 698
        height: 610
        x:378
        y:147

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

    function formatText(count,modelData){
        var data = modelData;
        return data.toString().length < 2?"0"+data:data;
    }

    Component{
        id: delegateComponent
        Label{
            text: rect.formatText(Tumbler.tumbler.count,modelData)
            opacity: 1.0-Math.abs(Tumbler.displacement)/(Tumbler.tumbler.visibleItemCount/2)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 30
            color: Tumbler.displacement == 0?"#00add7":"#000000"
        }
    }

    Rectangle{
        x:1157
        y:147
        width: 375
        height: 375
        radius: 22
        Image {
            anchors.fill: parent
            source: "qrc:/images/timebg.png"
        }

        Label{
            anchors.horizontalCenter: parent.horizontalCenter
            y:10
            font.pixelSize: 40
            text:qsTr("时间设置")
        }
        Rectangle{
            x:0
            y:190
            width: parent.width
            height: 55
            color: "#f2f2f2"
            border.width: 1
            border.color: "#cccccc"
        }
        Row{
            x:30
            y:80
            width: parent.width
            height: parent.height-y
            Tumbler{
                width: 120
                height: 270
                id: amPmTumbler
                model: [qsTr("上午AM"),qsTr("下午PM")]
                delegate: delegateComponent
            }
            Tumbler{
                width: 100
                height: 270
                id: hoursTumbler
                model: 12
                delegate: delegateComponent
            }
            Tumbler{
                width: 100
                height: 270
                id: minutesTumbler
                model: 60
                delegate: delegateComponent
            }
        }
    }

/*
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
        text: qsTr("保存")

        onClicked: {
            var setStr = calendar.selectedDate.toLocaleDateString(Qt.locale("en_US"),"yyyy-MM-dd")+" "+spHour.value+":"+spMinute.value+":"+spSecond.value
            console.debug("set--",setStr)
            ExGlobal.setTime(setStr)
        }
    }
*/
    Bt2 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.right: parent.right
        anchors.rightMargin: 170
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        image: "qrc:/images/cancelbt.png"
        text:qsTr("取消")
        onClicked: {
            Sequence.setTitle("setup");
            mainView.pop();
        }
    }

    Bt2{
        id: btconfirm
        anchors.bottom: btCannel.top
        anchors.bottomMargin: 30
        anchors.horizontalCenter: btCannel.horizontalCenter
        height: 106
        width: 299
        textoffsetx: 30
        textoffsety: -4
        textcolor: "#ffffff"
        image: "qrc:/images/confirmbt2.png"
        text:qsTr("确认")
        onClicked: {
            var setStr = calendar.selectedDate.toLocaleDateString(Qt.locale("en_US"),"yyyy-MM-dd")+" "
            if (amPmTumbler.currentIndex === 0)
                setStr += hoursTumbler.currentIndex;
            else
                setStr += hoursTumbler.currentIndex+12;
            setStr += ":"+minutesTumbler.currentIndex;
            console.debug("set--",setStr)
            ExGlobal.setTime(setStr)
            Sequence.setTitle("setup");
            mainView.pop();
        }
    }

    Component.onCompleted: {
        console.debug(calendar.selectedDate.toLocaleDateString())
        Sequence.setTitle("setup_datetime")
        if (ExGlobal.sysLanguageCode == 1)
            calendar.locale = Qt.locale("en_US");

        var dt = new Date();
        console.debug(dt.getHours(),dt.getMinutes())
        if (dt.getHours() >= 12)
        {
            amPmTumbler.currentIndex = 1
            hoursTumbler.currentIndex = dt.getHours()-12
        }
        else
        {
            amPmTumbler.currentIndex = 0
            hoursTumbler.currentIndex = dt.getHours()
        }
        minutesTumbler.currentIndex = dt.getMinutes()
    }
}
