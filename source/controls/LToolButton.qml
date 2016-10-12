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
    property string text: ""
    color: UI.cMainLBg
    Image {
        id:img
        width: parent.width/2
        height: width
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: parent.source
    }
    LText{
        id: txt
        text: root.text
        width: parent.width
        anchors.left: parent.left
        anchors.top: img.bottom
        horizontalAlignment: Text.AlignHCenter
        color: UI.cWhite
        visible: root.text ==""? false:true
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
//            root.color = UI.cOperActBg
            txt.font.bold = true
        }
        onExited: {
//            root.color = UI.cMainLBg
            txt.font.bold = false
        }
    }
}
