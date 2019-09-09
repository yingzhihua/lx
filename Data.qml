import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    property string titlemsg: qsTr("历史数据")
    id: data_page

    ListModel{
        id:dataModel
        ListElement{dataTime:"12:01:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"小明"}
        ListElement{dataTime:"13:05:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"小红"}
        ListElement{dataTime:"14:06:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"校长"}
        ListElement{dataTime:"12:01:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"张小明"}
        ListElement{dataTime:"14:06:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"林校长"}
        ListElement{dataTime:"12:01:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"小明"}
        ListElement{dataTime:"13:05:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"小红"}
        ListElement{dataTime:"14:06:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"校长"}
        ListElement{dataTime:"12:01:01";dataDate:"2019/04/17";dataCode:"SLX 01079";dataInfo:"张小明"}
    }

    Column{
        Repeater{
            model: dataModel

            Rectangle{
                height: 90
                width: dataPage.width
                color: (model.index%2 == 1)?"darkgrey":"#ffffff"
                Grid{
                    rows: 2
                    columns: 2
                    rowSpacing: 2
                    columnSpacing: 30
                    anchors.bottom: parent.bottom
                    Text {
                        text: dataTime
                    }
                    Text {
                        text: dataCode
                    }
                    Text {
                        text: dataDate
                    }
                    Text {
                        text: dataInfo
                    }
                }
            }
        }
    }

    onTitlemsgChanged: headerMsg.text = titlemsg;
}
