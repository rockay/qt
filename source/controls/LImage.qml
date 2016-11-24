import QtQuick 2.0

Image{
    property string picname: ""

    onPicnameChanged: {
        if(picname=="" || source == "")
            return;
        var path = ryControl.getCachePicPat()+picname+".png";
        if(utilityControl.isFileExist(path)){
            source = "file:///"+path;
        }else{
            console.log("图片不存在，再下载"+path);
            networkControl.doDownload(source ,path, false);
        }
    }

    Component.onCompleted: {
        if(picname=="" || source == "")
            return;
        // 缓存图片
        var path = ryControl.getCachePicPat()+picname+".png";
        if(!utilityControl.isFileExist(path)){
            console.log("图片不存在，再下载"+path);
            networkControl.doDownload(source ,path, false);
        }else{
            // 如果存在图片，在后台检查更新
        }
    }
}
