#include "ocvimageprovider.h"

OCVImageProvider::OCVImageProvider(QObject *parent) : QObject(parent),
        QQuickImageProvider(QQuickImageProvider::Image) {
    image0 = QImage(":/assets/help.png");
//    image1 = QImage(":/assets/help.png");
}

QImage OCVImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {


//    Q_UNUSED(id);
//    if(size) {
//        *size = image.size();
//    }
//    if(requestedSize.width() > 0 && requestedSize.height() > 0) {
//        image = image.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
//    }
//    return image;

    QString baseId = id.split('/').first(); // 忽略参数
    if(requestedSize.width() > 0 && requestedSize.height() > 0){
        if(baseId == "image0") {
                return image0.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
        }
        else if(baseId == "image1") {
                return image1.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
        }
    }
    return QImage();
}

void OCVImageProvider::updateImage(const QImage& image, int index) {

    if(!image.isNull() && index == 0){
        this -> image0 = image;
        emit imageChanged(index);
    }
    else if(!image.isNull()) {
        this -> image1 = image;
        emit imageChanged(index);
    }
}



