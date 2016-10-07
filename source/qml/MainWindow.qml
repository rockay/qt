import QtQuick 2.7
import QtQuick.Window 2.2

import "qrc:/controls/"
import "qrc:/qml/message/"
import "qrc:/qml/friend/"
import "qrc:/qml/group/"
import "qrc:/qml/cloud/"
import "qrc:/js/UI.js" as UI
import "qrc:/js/API.js" as API

Window {
    id:mainform
    title: qsTr("圈图")
    flags: Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint| Qt.Window;
    color: UI.cTransparent


    Rectangle{
        anchors.fill: parent;
        color: UI.cMainBg
        // 左侧
        Rectangle{
            id:leftbar
            width: UI.fWMainL
            height: mainform.height-50
            anchors.left: parent.left
            anchors.top: parent.top
            color: UI.cMainLBg
            Grid {
                width: parent.width
                height:parent.height-exit.height
                anchors.left: parent.left
                anchors.top: parent.top

                columns: 1
                LToolButton { source: "qrc:/images/photo.png"; //API.user_photo ;
                    width: parent.width; height: UI.fHToolButton-20
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                        }
                    }
                }
                LToolButton { source: "qrc:/images/message.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("会话")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= true
                            friend.visible= false
                            group.visible = false
                            cloud.visible = false
                        }
                    }
                }
                LToolButton { source: "qrc:/images/friend.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("好友")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= true
                            group.visible = false
                            cloud.visible = false
                        }
                    }
                }
                LToolButton { source: "qrc:/images/group.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("群组")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= false
                            group.visible = true
                            cloud.visible = false
                        }
                    }
                }
                LToolButton { source: "qrc:/images/cloud.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("云库")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= false
                            group.visible = false
                            cloud.visible = true
                        }
                    }
                }
            }

            Rectangle{
                id: exit
                width: UI.fWMainL
                height: height
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                LToolButton { source: "qrc:/images/cloud.png"; width: parent.width; height: UI.fHToolButton
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            main.hide();
                            login.show();
                        }
                    }
                }
            }
        }
        Rectangle{
            id:subview
            anchors.left: leftbar.right
            anchors.top: parent.top
            width: parent.width-leftbar.width
            height: parent.height
            color: UI.cTransparent
            Message{
                id: message
                anchors.fill: parent
                visible: true
            }
            Friend{
                id: friend
                anchors.fill: parent
                visible: false
            }
            Group{
                id:group
                anchors.fill: parent
                visible: false
            }
            Cloud{
                id:cloud
                anchors.fill: parent
                visible: false
            }

        }

        Rectangle{
            id:operWinArea
            width: UI.fWOperWin
            height: UI.fHOperWin
            anchors.right: parent.right
            anchors.top: parent.top
            color: parent.color
            Grid {
                columns: 3
                spacing: 0
                LOperButton { source: "qrc:/images/min.png"; width: operWinArea.width*3/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            mainform.visibility = Window.Minimized
                        }
                    }
                }
                LOperButton { source: "qrc:/images/max.png"; width: operWinArea.width*3/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if (mainform.visibility === Window.FullScreen){
                                parent.source = "qrc:/images/max.png";
                                mainform.visibility = Window.AutomaticVisibility
                            }
                            else{
                                mainform.visibility = Window.FullScreen
                                parent.source = "qrc:/images/min.png";
                            }
                        }
                    }
                }
                LOperButton { source: "qrc:/images/close.png"; width: operWinArea.width*4/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            Qt.quit()
                        }
                    }
                }
            }
        }
    }

}
