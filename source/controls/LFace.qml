import QtQuick 2.0
import org.lt.controls 1.1
import "qrc:/js/UI.js" as UI
import "qrc:/js/convertuni.js" as ConvertJS

Rectangle {
    id:mainPage
    width: grid.width;
    height: grid.height
    signal sigFaceClicked(string text);
    opacity: 1.0

    Component{
        id:imgc
        Rectangle {
            id:item
            width: grid.cellWidth; height: grid.cellHeight
            border.width: 1
            border.color: "#EAEAEA"
            radius: 2
            smooth: true
            color: "#FFFFFF"
//            AnimatedImage { source: path;width: parent.width;
//                height: parent.height; fillMode: Image.PreserveAspectFit; anchors.centerIn: parent;smooth: true }
            LText{
                id: emotiontxt
                font.family: UI.emojiFont
                pointSize: UI.BigFontPointSize
                anchors.centerIn: parent
                verticalAlignment: Text.AlignVCenter
                textFormat: Text.RichText
                text: "<img src='qrc:/images/emoji/drawable-xhdpi/u"+emojicode.replace("0x","")+".png' width=25 height=25/>"
            }

            MouseArea{
                id:ma
                anchors.fill: parent
                hoverEnabled: true
                onExited: {
                    item.opacity = 1.0;
                    item.color = "#FFFFFF"
                    mainHideCallBoard();
                }
                onEntered: {
                    item.opacity = 0.5;
                    item.color = "#EAEAEA"
                    var obj = mapToItem(null,x,y);
                    var posx = obj.x ;
                    var posy = obj.y ;
//                    mainShowCallBoard(title,posx,posy);
                }
                onClicked: {
                    sigFaceClicked(emotiontxt.text)
                    console.log("face:"+emotiontxt.text);
                    ppFace.hide()
                }
            }

        }
    }
    GridView {
        id:grid
        width: 35*13+5;
        height: 35*10
        cellWidth:35; cellHeight:35
        anchors.fill: parent
        model: FaceModel {}
        delegate: imgc
        focus: true
        boundsBehavior:Flickable.StopAtBounds
    }
    ChatTipsImage{
        id:imgBoard
        visible: false
        z:20
    }
    function mainShowCallBoard(number,posx,posy)
    {
        //console.log(number+"\n"+posx+"\n"+posy)
         var objxy = mapFromItem(null,posx,posy);
        var nx = objxy.x;
        var ny = objxy.y;
        if(nx<0)
        {
            nx = 0;
        }
        if(nx + imgBoard.width> mainPage.width)
        {
            nx = mainPage.width - imgBoard.width;
        }
        imgBoard.x = nx+6;
        imgBoard.y = ny+30;
        imgBoard.setCurrentTxt(number);
        imgBoard.visible = true;
    }
    function mainHideCallBoard()
    {
        imgBoard.visible = false;
    }
}

