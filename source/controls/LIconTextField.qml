import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "qrc:/js/UI.js" as UI

Item {

    property string svgsrc: ""
    property string svgcsrc: ""
    property bool ispwd: false
    property string value: inputArea.text
    property string dispTxt: ""
    property string defaultValue: ""
    property int radis: 0
    id: root
    Rectangle{
        id: bg
        anchors.fill: parent
        color: UI.cTBBg
        radius: radis
        border.color: UI.cTBBorder
        border.width: 1
        TextField {
            id: inputArea
            width:  parent.width-svg.width-1
            height: parent.height-2
            anchors.left: parent.left
            anchors.leftMargin: 1
            anchors.top: parent.top
            anchors.topMargin: 1
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignLeft
            font.pointSize:UI.TinyFontPointSize
            font.family: UI.defaultFontFamily
            placeholderText: dispTxt
            echoMode: ispwd ? TextInput.Password : TextInput.Normal
            text: defaultValue
            style: TextFieldStyle{
                textColor: UI.cBlack
                placeholderTextColor:UI.cFTB
                background: Rectangle {
                    implicitWidth: inputArea.width
                    implicitHeight: inputArea.height
                    border.color: UI.cTBBorder
                    border.width: 0
                    color: UI.cTBBg
                }
            }
            onTextChanged: {
                root.defaultValue = text
               if(text.length>0){
                   image.source = svgcsrc
               }else{
                   image.source = svgsrc
               }
            }
        }


        Rectangle{
            id: svg
            width: root.height
            height: root.height
            anchors.top: parent.top
            anchors.right: parent.right
            color: UI.cTransparent
            Image {
                id:image
                width: parent.width/3
                height: parent.height/3
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: svgsrc
            }
            MouseArea{
                anchors.fill: parent
                cursorShape: inputArea.text.length>0?Qt.PointingHandCursor:Qt.ArrowCursor;
                onClicked: {
                    if(inputArea.text.length>0)
                        inputArea.text =""

                }
            }
        }

//        Image {
//            id: svg
//            source: svgsrc
//            width: parent.height
//            height: parent.height
//            sourceSize.width: parent.height*2/5
//            sourceSize.height: parent.height*2/5
//            anchors.top: parent.top
//            anchors.right: parent.right
//            anchors.rightMargin: parent.height*1.5/5
//            anchors.topMargin: parent.height*1.5/5
//        }
    }
}
