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
                LToolButton { source: API.user_photo ;
                    width: parent.width; height: UI.fHToolButton-20
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                        }
                    }
                }
                LToolButton { id:msgbtn; source: "qrc:/images/icon/message.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("会话")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= true
                            friend.visible= false
                            group.visible = false
                            cloud.visible = false
                            msgbtn.source = "qrc:/images/icon/messagep.png"
                            frdbtn.source = "qrc:/images/icon/friend.png"
                            grpbtn.source = "qrc:/images/icon/group.png"
                            cldbtn.source = "qrc:/images/icon/cloud.png"
                            if(!message.isLoad)
                                message.isLoad = true
                        }
                    }
                }
                LToolButton {id:frdbtn; source: "qrc:/images/icon/friend.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("好友")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= true
                            group.visible = false
                            cloud.visible = false
                            msgbtn.source = "qrc:/images/icon/message.png"
                            frdbtn.source = "qrc:/images/icon/friendp.png"
                            grpbtn.source = "qrc:/images/icon/group.png"
                            cldbtn.source = "qrc:/images/icon/cloud.png"
                            if(!friend.isLoad)
                                friend.isLoad = true
                        }
                    }
                }
                LToolButton {id:grpbtn; source: "qrc:/images/icon/group.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("群组")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= false
                            group.visible = true
                            cloud.visible = false
                            msgbtn.source = "qrc:/images/icon/message.png"
                            frdbtn.source = "qrc:/images/icon/friend.png"
                            grpbtn.source = "qrc:/images/icon/groupp.png"
                            cldbtn.source = "qrc:/images/icon/cloud.png"
                            if(!group.isLoad)
                                group.isLoad = true
                        }
                    }
                }
                LToolButton {id:cldbtn; source: "qrc:/images/icon/cloud.png"; width: parent.width; height: UI.fHToolButton
                    text: qsTr("云库")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            message.visible= false
                            friend.visible= false
                            group.visible = false
                            cloud.visible = true
                            msgbtn.source = "qrc:/images/icon/message.png"
                            frdbtn.source = "qrc:/images/icon/friend.png"
                            grpbtn.source = "qrc:/images/icon/group.png"
                            cldbtn.source = "qrc:/images/icon/cloudp.png"
                            if(!cloud.isLoad)
                                cloud.isLoad = true
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
                anchors.bottomMargin: 10
                LToolButton {id:exitbtn; source: "qrc:/images/icon/exit.png"; width: parent.width; height: UI.fHToolButton
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            main.hide();
                            login.show();
                        }
                        onEntered: {
                            exitbtn.source ="qrc:/images/icon/exitp.png"
                        }
                        onExited: {
                            exitbtn.source ="qrc:/images/icon/exit.png"
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
                LOperButton { source: "qrc:/images/icon/min.png"; width: operWinArea.width*3/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            mainform.visibility = Window.Minimized
                        }
                    }
                }
                LOperButton { source: "qrc:/images/icon/max.png"; width: operWinArea.width*3/10; height: operWinArea.height
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if (mainform.visibility === Window.FullScreen){
                                parent.source = "qrc:/images/icon/max.png";
                                mainform.visibility = Window.AutomaticVisibility
                            }
                            else{
                                mainform.visibility = Window.FullScreen
                                parent.source = "qrc:/images/icon/max.png";
                            }
                        }
                    }
                }
                LOperButton { source: "qrc:/images/icon/close.png"; width: operWinArea.width*4/10; height: operWinArea.height
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
