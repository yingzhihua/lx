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
        x:100
        y:0
        height: 800
        width: 1200
        legend.visible: false        
        antialiasing: true

        ValueAxis{
            id: axisX
            min: 0
            max: 50
            tickCount: 11
            minorTickCount: 4
            labelFormat: "%d"
            titleText: "cycle"
        }

        ValueAxis{
            id:axisY
            min:0
            max:200
            tickCount: 11
            minorTickCount: 4
            labelFormat: "%d"
            titleText: "brightness"
        }
    }

    Grid{
        y:80
        anchors.left: chartView.right
        anchors.leftMargin: 80
        flow: Grid.TopToBottom
        rows: 15
        columnSpacing: 80
        Repeater{
            model: testResultModel
            Text{
                text: cycle+", "+TestValue
            }
        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        onClicked: {
            mainView.pop();
        }
    }

    function setChart(){
        chartView.removeAllSeries();
        chartView.title = testModel.getCurrTestPanelName();

        for (var i = 0; i < testResultModel.rowCount(); i++){
            var seriesName = "R"+Math.floor(testResultModel.getField(i,"PosIndex")/11+1)+"C"+(testResultModel.getField(i,"PosIndex")%11+1);
            var newline =chartView.series(seriesName);
            var value = testResultModel.getField(i,"TestValue");
            var cycle = testResultModel.getField(i,"cycle");
            //console.debug("setChart,",value)
            if (newline === null)
            {
                newline =chartView.createSeries(ChartView.SeriesTypeLine,seriesName);
                newline.axisX = axisX;
                newline.axisY = axisY;
            }
            if (value > axisY.max - 10)
                axisY.max = value + 10;
            if (cycle > axisX.max)
                axisX.max = cycle;

            newline.append(cycle,value);
        }
    }

    Component.onCompleted: {
        Sequence.setTitle("datamenu");
        setChart()
    }
}
