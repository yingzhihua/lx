import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3
import Dx.Global 1.0
import "../components"
Rectangle {
    property int devindex: 0
    property int ax: 0
    id:rect

    function formatText(count,modelData){
        var data = modelData;
        return data.toString().length < 2?"0"+data:data;
    }

    FontMetrics{
        id: fontMetrics
    }

    Component{
        id: delegateComponent
        Label{
            text: rect.formatText(Tumbler.tumbler.count,modelData)
            opacity: 1.0-Math.abs(Tumbler.displacement)/(Tumbler.tumbler.visibleItemCount/2)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: fontMetrics.font.pixelSize*1.25
        }
    }

    GroupBox{
        id:dev
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        width: 400

        Row{
            anchors.fill: parent
            spacing: 20
            RadioButton{
                id:dev1
                text:qsTr("前帕尔贴")
                checked: true
                onClicked: devindex = 0
            }
            RadioButton{
                id:dev2
                text:qsTr("后帕尔贴")
                onClicked: devindex = 1
            }
            RadioButton{
                id:dev3
                text: qsTr("预热")
                onClicked: devindex = 2
            }
        }
    }

    Label{
        id:lkp
        anchors.left: dev.right
        anchors.leftMargin: 50
        anchors.verticalCenter: dev.verticalCenter
        text: "Kp:"
    }
    Tumbler{
        id:k1
        anchors.left: lkp.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lkp.verticalCenter
        model: 100
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }
    Label{
        id:kdot
        anchors.left: k1.right
        anchors.leftMargin: 3
        anchors.verticalCenter: k1.verticalCenter
        text:"."
    }

    Tumbler{
        id:k2
        anchors.left: kdot.right
        anchors.leftMargin: 3
        anchors.verticalCenter: kdot.verticalCenter
        model:100
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }

    Label{
        anchors.left: k2.right
        anchors.leftMargin: 20
        anchors.verticalCenter: k2.verticalCenter
        id:lki
        text: "Ki:"
    }
    Tumbler{
        id:i1
        anchors.left: lki.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lki.verticalCenter
        model: 100
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }

    Tumbler{
        id:i2
        anchors.left: i1.right
        anchors.verticalCenter: i1.verticalCenter
        model:100
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }


    Label{
        anchors.left: i2.right
        anchors.leftMargin: 20
        anchors.verticalCenter: i2.verticalCenter
        id:lkd
        text: "Kd:"
    }
    Tumbler{
        id:d1
        anchors.left: lkd.right
        anchors.leftMargin: 10
        anchors.verticalCenter: lkd.verticalCenter
        model: 100
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }

    Tumbler{
        id:d2
        anchors.left: d1.right
        anchors.verticalCenter: d1.verticalCenter
        model:100
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }

    Button{
        id:pidset
        anchors.left: d2.right
        anchors.leftMargin: 50
        anchors.verticalCenter: d2.verticalCenter
        text: "设置"
    }

    CheckBox{
        id:cbChannel1
        anchors.left: pidset.right
        anchors.leftMargin: 80
        anchors.verticalCenter: pidset.verticalCenter
        text: "前帕尔贴"
    }
    CheckBox{
        id:cbChannel2
        anchors.left: cbChannel1.right
        anchors.leftMargin: 20
        anchors.verticalCenter: cbChannel1.verticalCenter
        text: "后帕尔贴"
    }
    CheckBox{
        id:cbChannel3
        anchors.left: cbChannel2.right
        anchors.leftMargin: 20
        anchors.verticalCenter: cbChannel1.verticalCenter
        text: "预热"
    }

    Label{
        anchors.left: cbChannel3.right
        anchors.leftMargin: 20
        anchors.verticalCenter: cbChannel3.verticalCenter
        id:ltempAir
        text: "目标温度:"
    }
    Tumbler{
        id:tempAir
        anchors.left: ltempAir.right
        anchors.leftMargin: 10
        anchors.verticalCenter: ltempAir.verticalCenter
        model: 120
        visibleItemCount: 3
        height: 80
        width: 30
        delegate: delegateComponent
    }
    Button{
        id:openTemp
        anchors.left: tempAir.right
        anchors.leftMargin: 50
        anchors.verticalCenter: tempAir.verticalCenter
        text: "打开温控"
    }
    Button{
        id:closeTemp
        anchors.left: openTemp.right
        anchors.leftMargin: 20
        anchors.verticalCenter: openTemp.verticalCenter
        text: "关闭温控"
    }
    Button{
        id:clearLine
        anchors.left: closeTemp.right
        anchors.leftMargin: 20
        anchors.verticalCenter: closeTemp.verticalCenter
        text: "清除曲线"
    }

    ChartView {
        id: chartView
        title: "实时温度曲线"
        anchors.top: dev.bottom
        anchors.topMargin: 20
        height: 600
        width: 800
        legend.alignment: Qt.AlignTop
        animationOptions: ChartView.SeriesAnimations
        antialiasing: true

        ValueAxis{
            id: axisX
            min: 0
            max: 50
            tickCount: 5
        }
        ValueAxis{
            id:axisY
            min:-0.5
            max:1.5
        }

        LineSeries{
            id:serial0
            name:"前帕尔贴"

        }
        LineSeries{
            id:serial1
            name:"后帕尔贴"
        }
    }

    Timer{
        id: timer
        interval: 700
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: {
            ax++;
            serial0.append(ax,Math.random());
            serial1.append(ax,Math.random());
        }
    }
    Component.onCompleted: {
        timer.start()
    }
}
