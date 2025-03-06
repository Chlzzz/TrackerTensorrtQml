#ifndef IMAGEPROCESS_H_
#define IMAGEPROCESS_H_



#include <QImage>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QImage>
#include <QThread>
#include <QFileInfo>

#include <iostream>
#include <cstring>
#include <opencv2/highgui.hpp>

#include "utility.h"
#include "app_rtdetr.h"

#ifdef _DEBUG
    #include <QDebug>
#endif

class ImageProcess : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(ImageProcess)

public:
    explicit ImageProcess(QObject* parent = nullptr);
    ~ImageProcess();

    QImage MatImageToQImage(const cv::Mat& src);
    void initCapture(const std::string cameraIndex, const double capWidth, const double capHeight);

signals:
    void sendImage(const QImage&);
    void sendCameraError(const QString);
    void sendInferDeviceError(const QString);
    void sendInferDeviceSuccess();

public slots:
    void startCapture();
    void readFrame();
    void endCapture();

    void initengine();

    void checkInferParameter(QVecInt capturePara, QStringList inferPara);
    void changeNNStatus();

private:
    QMutex mutex;
    QMutex image_process_mutex;

    cv::VideoCapture cap;
    cv::Mat m_frame;
    QImage m_q_frame;

    bool m_image_process_running;
    bool m_nn_running;
    bool m_full_demo_running;

    std::string m_camera_index;
    int m_capture_width;
    int m_capture_height;

    QString m_full_network_path;
    QString runtime_error;

    // CPUInfer *infer;
    std::shared_ptr<AppRTdetr> m_infer;
    
};

#endif // IMAGEPROCESS_H_
