import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"
Page {
    property var itemList:[]
    ChartView {
        id: chartView
        title: "实时测试曲线"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: +100
        anchors.verticalCenter: parent.verticalCenter
        height: 700
        width: 1000
        legend.visible: true
        animationOptions: ChartView.SeriesAnimations
        antialiasing: true

        ValueAxis{
            id: axisX
            min: 0
            max: 40
            tickCount: 5
            minorTickCount: 9
            labelFormat: "%d"
        }

        CategoryAxis{
            id:axisY
            min:-10
            max:30
            CategoryRange{
                label:"0"
                endValue: 0
            }
            CategoryRange{
                label:ExGlobal.getItemCT(testResultModel.getCurrItemId())/2
                endValue: ExGlobal.getItemCT(testResultModel.getCurrItemId())/2
            }
            CategoryRange{
                label:"CT "+ExGlobal.getItemCT(testResultModel.getCurrItemId())
                endValue: ExGlobal.getItemCT(testResultModel.getCurrItemId())
            }
            CategoryRange{
                label:ExGlobal.getItemCT(testResultModel.getCurrItemId())*2
                endValue: ExGlobal.getItemCT(testResultModel.getCurrItemId())*2
            }
            labelsPosition: CategoryAxis.AxisLabelsPositionOnValue
        }
    }

    Column{
        id: gridIitem
        anchors.verticalCenter: chartView.verticalCenter
        anchors.right: chartView.left
        anchors.rightMargin: 100

        Repeater{
            model: itemList.length
            Rectangle{
                width: 200
                height: 50

                Text {
                    text:"H"+Math.floor(itemList[modelData]/1000/11+1)+"N"+Math.floor(itemList[modelData]/1000%11+1)
                }
                Text {
                    x:100
                    text: itemList[modelData]%1000==0?"ND":itemList[modelData]%1000/10
                }
            }

        }
    }

    Bt1 {
        id: btCannel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            if (ExGlobal.dataEntry() === 0)
                dataView.pop();
            else
                stackView.pop();
        }
    }
    function setChart(){
        chartView.removeAllSeries();
        chartView.title = ExGlobal.getPosName(testResultModel.getCurrItemId());

        for (var i = 0; i < testResultModel.rowCount(); i++){
            var seriesName = "H"+Math.floor(testResultModel.getField(i,"PosIndex")/11+1)+"N"+(testResultModel.getField(i,"PosIndex")%11+1);
            var newline =chartView.series(seriesName);
            var value = testResultModel.getField(i,"TestValue");
            var cycle = testResultModel.getField(i,"cycle");
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
        itemList = ExGlobal.getCurrItemResult()
        setChart()
    }
}
