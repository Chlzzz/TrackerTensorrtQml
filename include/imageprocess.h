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
#include <thread>
#include <opencv2/highgui.hpp>

#include "utility.h"
#include "node_rtdetr.h"
#include "node_ostrack.h"

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
    void ostrack_init(const QRect &roi);

signals:
    void sendImage(const QImage&);
    void sendCameraError(const QString);
    void sendInferDeviceError(const QString);
    void sendInferDeviceSuccess();

public slots:
    void startCapture();
    void readFrame();
    void endCapture();

    void initengine(QStringList capturePara, QStringList inferPara);

//    void checkInferParameter(QStringList capturePara, QStringList inferPara);
    void changeNNStatus();

private:
    QMutex mutex;
    QMutex image_process_mutex;

    cv::VideoCapture cap;
    cv::Mat m_frame;
    cv::Rect init_bbox;
    QImage m_q_frame;

    bool m_image_process_running;
    bool m_nn_running;
    bool m_full_demo_running;

    QString m_camera_type;
    QString m_camera_index;
//    int m_capture_width;
//    int m_capture_height;
    QString m_task_type;
    QString m_full_network_path;
    QString runtime_error;

    bool is_init;

    // CPUInfer *infer;
   std::shared_ptr<AppRTdetr> m_infer;
   std::shared_ptr<AppOStrack>  m_tracker;
    
};

#endif // IMAGEPROCESS_H_
