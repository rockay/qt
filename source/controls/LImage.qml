import QtQuick 2.0

Image{
    property string picname: ""

//    onPicnameChanged: {
//        var path = ryControl.getCachePicPat()+picname+".png";
//        if(utilityControl.isFileExist(path)){
//            source = "file:///"+path;
//            console.log("image file exist:"+path)
//        }
//    }

//    Component.onCompleted: {
//        // 缓存图片
//        var path = ryControl.getCachePicPat()+picname+".png";
//        if(!utilityControl.isFileExist(path)){
//            networkControl.doDownload(source ,path);
//        }
//    }
}
