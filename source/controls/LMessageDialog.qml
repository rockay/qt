import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import "qrc:/controls/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API

Window {
    id: tipWindow
    title: qsTr("圈图")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    modality:  Qt.ApplicationModal
    color: UI.cTransparent
    width: 200
    height: 130

    signal okClicked();
    signal cancelClicked();
    property string tiltle: "温馨提示"
    property string msg: ""
    property int flag: 1
    property string okTitle: qsTr("确定")
    property string cancelTitle: qsTr("取消")


    Rectangle{
        anchors.fill: parent
        color: UI.cWhite
        border.width: 1
        border.color: "#AABA8D"

        Column {
            width: parent.width-4
            height: parent.height-4
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 2
            Rectangle {
                id: headrect1
                color: UI.cTransparent
                width: parent.width;
                height: 30
                Image{
                    id: iconimg
                    height: tiptitle.contentHeight
                    width: height
                    x: 5
                    y: 5
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/images/qt-logo.png"
                }

                LText{
                    id: tiptitle
                    anchors.left: iconimg.right
                    anchors.leftMargin: 5
                    y: 5
                    text: tiltle
                }

                LOperButton {
                    source: "qrc:/images/icon/close.png";
                    width: parent.height
                    height: width
                    anchors.right: parent.right
                    anchors.top: parent.top
                    bgcolor: UI.cTransparent
                    bdcolor: UI.cTransparent
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            tipWindow.hide();
                        }
                    }
                }
            }
            Rectangle {
                id: centerrect1
                color: UI.cTransparent
                width: parent.width;
                height: parent.height - headrect1.height - bottomrect1.height;
                LText{
                    anchors.left: parent.left
                    anchors.leftMargin: 15
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: msg
                }
            }
            Rectangle {
                id: bottomrect1
                color: "#F0F0F0"
                width: parent.width
                height: 40
                LButton{
                    id: okbtn
                    x: cancelbtn.width==0 ? (parent.width-width)/2 : 10
                    y: 3
                    width: 1&flag ? 80:0
                    height: parent.height - 6
                    visible: 1&flag
                    text: okTitle
                    radius: 0
                    onClicked: {
                        tipWindow.hide();
                        tipWindow.okClicked();
                    }
                }
                LButton{
                    id: cancelbtn
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    y: 3
                    width: 2&flag ? 80:0
                    height: parent.height - 6
                    visible: 2&flag
                    radius: 0
                    text: cancelTitle
                    onClicked: {
                        tipWindow.hide();
                        tipWindow.cancelClicked();
                    }
                }
            }
        }
    }
}
