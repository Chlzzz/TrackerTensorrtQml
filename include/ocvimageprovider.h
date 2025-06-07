#ifndef OCVIMAGEPROVIDER_H
#define OCVIMAGEPROVIDER_H

#include <QObject>
#include <QImage>
#include <QQuickImageProvider>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#ifdef _DEBUG
    #include <QDebug>
#endif

class OCVImageProvider : public QObject, public QQuickImageProvider {
    Q_OBJECT

public:
    explicit OCVImageProvider(QObject* parent = nullptr);
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

public slots:

    void updateImage(const QImage& image, int index);

signals:
    void imageChanged(int index);

private:
    QImage image0, image1;
    QMutex m_mutex;
};

#endif // OCVIMAGEPROVIDER_H
