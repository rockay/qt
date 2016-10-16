import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQml.Models 2.2

import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API

Rectangle{
    color: UI.cTransparent
    property ListModel chatLists: chatList

    ListModel{
        id: chatList
    }
    SequentialAnimation {
        id:animation2
        NumberAnimation { target: viewImg; property: "opacity"; to: 1; duration: 500 }
        PauseAnimation { duration: 2000 }
    }
    Image{
        id: viewImg
        visible: false
        anchors.centerIn: parent
        z:listView.z+1
        MouseArea{
            anchors.fill: parent
            onClicked: {
                viewImg.visible = false;
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: UI.fChatMargin
            displayMarginBeginning: 40
            displayMarginEnd: 40
            verticalLayoutDirection: ListView.BottomToTop
            spacing: 12
            model: chatList
            delegate: Column {
                anchors.right: sentByMe ? parent.right : undefined
                spacing: 6

                readonly property bool sentByMe: recipient == "Me"

                Row {
                    id: messageRow
                    spacing: 6
                    anchors.right: sentByMe ? parent.right : undefined

                    Image {
                        id: you
                        height: UI.fChatImgH
                        width: height
                        source: !sentByMe ? API.photoObjMap[author] : ""
                    }

                    Rectangle {
                        width: Math.min(ctype == 1 ? (messageText.implicitWidth+ 24 ) : ctype == 2 ? (messageImg.width + 24) : (messageFile.width + 24)
                                        , listView.width - 2*UI.fChatImgH - messageRow.spacing-24)
                        height: ctype == 1 ? messageText.implicitHeight + 24 : ctype == 2 ? (messageImg.height + 24) : (messageFile.height + 24)
                        color: sentByMe ? UI.cRightBg : UI.cWhite
                        border.color: UI.cLeftBorder
                        border.width: 2
                        anchors.leftMargin:  sentByMe ? UI.fChatImgH : 0
                        anchors.rightMargin: !sentByMe ?UI.fChatImgH : 0

                        Label {
                            id: messageText
                            text: message
                            color: sentByMe ? UI.cChatFont : UI.cChatFont
                            anchors.fill: parent
                            anchors.margins: 12
                            wrapMode: Label.Wrap
                            visible: ctype == 1 ? true : false
                        }
                        Image {
                            id: messageImg
                            width: UI.fChatImgH*3
                            height: UI.fChatImgH*4
                            anchors.margins: 12
                            anchors.centerIn: parent
                            source: ctype == 2?message:""
                            visible: ctype == 2 ? true : false
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    console.log("预览图片")
                                    viewImg.source = messageImg.source;
                                    viewImg.visible = true;
                                    animation2.stop()
                                    animation2.start()
                                }
                            }
                        }
                        Image {
                            id: messageFile
                            width: UI.fChatImgH*3
                            height: UI.fChatImgH*4
                            anchors.margins: 12
                            anchors.centerIn: parent
                            source: ctype == 3?message:""
                            visible: ctype == 3 ? true : false
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    console.log("预览文件")
                                }
                            }
                        }
                    }
                    Rectangle{
                        height: UI.fChatImgH
                        width: height+UI.fChatMargin
                        Image {
                            id: me
                            width: parent.height
                            height: parent.height
                            source: sentByMe ? API.user_photo : ""
                        }
                    }
                }

                Label {
                    id: timestampText
//                    text: Qt.formatDateTime(timestamp, "d MMM hh:mm")
                    text: timestamp
                    color: UI.cTime
                    horizontalAlignment: Text.Center
                    width: listView.width
                    anchors.right: sentByMe ? parent.right : undefined

                }
            }

            ScrollBar.vertical: ScrollBar {}
        }
    }
}
