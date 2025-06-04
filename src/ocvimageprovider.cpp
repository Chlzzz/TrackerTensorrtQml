#include "ocvimageprovider.h"

OCVImageProvider::OCVImageProvider(QObject *parent) : QObject(parent),
        QQuickImageProvider(QQuickImageProvider::Image) {
    image1 = QImage(":/assets/help.png");
    image2 = QImage(":/assets/help.png");
}

QImage OCVImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id);

    if(size) {
        *size = image1.size();
    }

    if(id == "camera1") {
        if(requestedSize.width() > 0 && requestedSize.height() > 0) {
            return image1.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
        }
    } else if(id =="camera2") {
        if(requestedSize.width() > 0 && requestedSize.height() > 0) {
            return image2.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
        }
    }
}

void OCVImageProvider::updateImage(const QImage& image, int camIndex) {
    if(!image.isNull()){
        if(camIndex == 1 && this -> image1 != image){
            this -> image1 = image;
            emit imageChanged(camIndex);
        } else if(camIndex == 2 && this -> image2 != image){
            this -> image2 = image;
            emit imageChanged(camIndex);
        }
    }
}
