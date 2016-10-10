import QtQuick 2.1
import QtQuick.XmlListModel 2.0

XmlListModel {
    id: xmlModel
    source:"qrc:/images/ybemxml_new.xml"
    query: "/ybdb/img/item"
    XmlRole { name: "title"; query: "title/string()" }
    XmlRole { name: "path"; query: "path/string()" }
}
