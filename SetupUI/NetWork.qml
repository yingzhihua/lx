import QtQuick 2.12
import QtQuick.Controls 2.5

import Dx.Sequence 1.0
import Dx.Global 1.0

import "../components"

Page {
    id: boxready_page
    Rectangle{
        anchors.fill: parent
        color: "#f5f5f5"

        Rectangle{
            width: 900
            height: 400
            anchors.centerIn: parent
            //border.color: "darkgray"
            radius: 22
            clip: true

            Grid{
                rows: 3
                columns: 5
                spacing: 10
                rowSpacing: 30
                anchors.centerIn: parent
                Text{width: 150; font.pixelSize: 40;text: "IP地址";rightPadding:20;horizontalAlignment: Text.AlignRight}
                //TextField{id:ip1;width: 120;background: Rectangle{border.color: "darkgray";color: "#e6e6e6"}}
                TextField{id:ip1;width: 120}
                TextField{id:ip2;width: ip1.width}
                TextField{id:ip3;width: ip1.width}
                TextField{id:ip4;width: ip1.width}
                Text{width: 150; text: "子网掩码";font.pixelSize: 40;rightPadding:20;horizontalAlignment: Text.AlignRight}
                TextField{id:mask1;width: ip1.width}
                TextField{id:mask2;width: ip1.width}
                TextField{id:mask3;width: ip1.width}
                TextField{id:mask4;width: ip1.width}
                Text{width: 150; text: "网关";font.pixelSize: 40;rightPadding:20;horizontalAlignment: Text.AlignRight}
                TextField{id:gate1;width: ip1.width}
                TextField{id:gate2;width: ip1.width}
                TextField{id:gate3;width: ip1.width}
                TextField{id:gate4;width: ip1.width}
            }
        }

        Bt1 {
            id: btCannel
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40
            anchors.right: parent.right
            anchors.rightMargin: 120
            onClicked: {
                var ip = ip1.text+"."+ip2.text+"."+ip3.text+"."+ip4.text;
                var mask = mask1.text+"."+mask2.text+"."+mask3.text+"."+mask4.text;
                var gate = gate1.text+"."+gate2.text+"."+gate3.text+"."+gate4.text;
                ExGlobal.setNetWork(ip,mask,gate);
                mainView.pop();
                Sequence.hideTitle(true);
            }
        }
    }

    Component.onCompleted: {
        Sequence.hideTitle(false);
        var net = ExGlobal.getNetWork();
        ip1.text = net[0];
        ip2.text = net[1];
        ip3.text = net[2];
        ip4.text = net[3];
        gate1.text = net[4];
        gate2.text = net[5];
        gate3.text = net[6];
        gate4.text = net[7];
        mask1.text = net[8];
        mask2.text = net[9];
        mask3.text = net[10];
        mask4.text = net[11];
    }
}
