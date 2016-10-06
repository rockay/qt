/****************************************************
 * 名称：主面板左侧几个切换按钮
 * 作者：LT
 * 日期：2016.10.1
 * 修改记录：
 *
 ****************************************************/
import QtQuick 2.0
import "qrc:/js/UI.js" as UI

Rectangle{
    id:root
    property string source:""
    color: UI.cMainLBg
    Image {
        width: parent.width/2
        height: parent.height/2
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: parent.source
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
//            root.color = UI.cOperActBg
        }
        onExited: {
            root.color = UI.cMainLBg
        }
    }
}
