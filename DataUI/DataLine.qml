import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    ChartView {
        id: chartView
        title: "实时测试曲线"
        anchors.centerIn: parent
        height: 600
        width: 860
        legend.visible: false
        animationOptions: ChartView.SeriesAnimations
        antialiasing: true

        ValueAxis{
            id: axisX
            min: 0
            max: 50
            tickCount: 6
            minorTickCount: 9
            labelFormat: "%d"
        }

        ValueAxis{
            id:axisY
            min:0
            max:50
            labelFormat: "%d"
        }
    }
    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            dataView.pop();
        }
    }
    function setChart(){
        chartView.removeAllSeries();
        chartView.title = ExGlobal.getPosName(testResultModel.getCurrItemId());
    }

    Component.onCompleted: {
        setChart()
    }
}
