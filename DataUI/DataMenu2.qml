import QtQuick 2.12
import QtQuick.Controls 2.5
import Dx.Global 1.0
import Dx.Sequence 1.0
Rectangle {
    id: data_page
    ScrollView{
        id:scroll
        y:200
        width: 530
        height: 300
        //clip:true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        //ScrollBar.vertical.interactive: true
        ListView{
            model:20
            delegate: ItemDelegate{
                text: "Item "+index
            }
        }
        ScrollBar{
            id:vbar
            hoverEnabled: true
            active: hovered||pressed
            orientation: Qt.Vertical
            size:100
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }
}
