#include "ocvimageprovider.h"

OCVImageProvider::OCVImageProvider(QObject *parent) : QObject(parent),
        QQuickImageProvider(QQuickImageProvider::Image) {
    image0 = QImage(":/assets/help.png");
    image1 = QImage(":/assets/help.png");
}

QImage OCVImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id);

    if(size) {
        *size = image0.size();
    }

    if(id.startsWith("image0")) {
        if(requestedSize.width() > 0 && requestedSize.height() > 0) {
            return image0.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
        }
    } else if(id.startsWith("image1")) {
        if(requestedSize.width() > 0 && requestedSize.height() > 0) {
            return image1.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
        }
    }
}

void OCVImageProvider::updateImage(const QImage& image, int camIndex) {
    if(!image.isNull()){
        if(camIndex == 0 && this -> image0 != image){
            this -> image0 = image;
            emit imageChanged(camIndex);
        } else if(camIndex == 1 && this -> image1 != image){
            this -> image1 = image;
            emit imageChanged(camIndex);
        }
    }
}
