import QtQuick 2.0
import QtQuick.Controls 2.0

import "qrc:/js/UI.js" as UI

MenuItem {
    id: mitemroot
    font.family: UI.defaultFontFamily
    property string bgcolor : UI.cTransparent
    focus: true
    hoverEnabled: true
    background: Item{
        Rectangle{
            x: 1
            y: 1
            width: parent.width-2
            height: parent.height-2
            color: mitemroot.hovered || mitemroot.down ? UI.cLightBlue : UI.cTransparent
        //        MouseArea{
        //            anchors.fill: parent
        //            hoverEnabled: true
        //            propagateComposedEvents: true
        //            onEntered: {
        //                parent.color = UI.cLightBlue;
        //            }
        //            onExited: {
        //                parent.color = UI.cTransparent;
        //            }
        //            onClicked: {
        ////                mouse.accepted = false
        //                mitemroot.triggered();
        //            }
        //        }
        }

    }
    leftPadding: 10

}

