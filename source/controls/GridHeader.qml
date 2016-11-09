
import QtQuick 2.0

Item {
    height: 60
    width: parent.width
    property GridView mainListView

    property bool refresh: state == "pulled" ? true : false

    Row {
        spacing: 6
        height: childrenRect.height
        anchors.centerIn: parent

        Image {
            id: arrow
            source: "qrc:/images/icon/refresh.png"
            transformOrigin: Item.Center
            Behavior on rotation { NumberAnimation { duration: 200 } }
        }


        LText {
            id: label
            anchors.verticalCenter: arrow.verticalCenter
            text: "下拉刷新"
            color: "#999999"
        }
    }

    states: [
        State {
            name: "base"; when: mainListView.contentY >= -120
            PropertyChanges { target: arrow; rotation: 180 }
        },
        State {
            name: "pulled"; when: mainListView.contentY < -120
            PropertyChanges { target: label; text: "放开刷新" }
            PropertyChanges { target: arrow; rotation: 0 }
        }
    ]
}
