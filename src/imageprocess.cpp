#include "imageprocess.h"

ImageProcess::ImageProcess(QObject* parent) : QObject(parent) {
    m_image_process_running = false;
    m_nn_running = false;
    m_full_demo_running = false;
    m_infer = std::make_shared<AppRTdetr>();
    m_tracker = std::make_shared<AppOStrack>();
}

ImageProcess::~ImageProcess(){

}

/// @brief convert opencv cv::Mat to qt QImage
/// @param src 
/// @return QImage
QImage ImageProcess::MatImageToQImage(const cv::Mat& src) {
    QImage returnImage;
    int channel = src.channels();
    switch(channel) {
        case 1: {
            QImage qImage(src.cols, src.rows, QImage::Format_Indexed8);     // CV_8UC1 grey sacle, create QImage from the given Mat CV_8UC1
            qImage.setColorCount(256);                                      // extend the colorlist to 256
            for(int i = 0; i < 256; i ++){                                  // set color to given index
                qImage.setColor(i,qRgb(i,i,i));                             // generate grayscale pic
            }
            uchar *pSrc = src.data;                                         // get the pointer of the src's data
            for(int row = 0; row < src.rows; row ++){                       // use the pointer to scan the pixels
                uchar* pDest = qImage.scanLine(row);
                std::memcpy(pDest, pSrc, src.cols);
                pSrc += src.step;            
            }
            return qImage;
        }
        break;

        case 4: {
            const uchar *pSrc = (const uchar*)src.data;                                     // return the created image, CV_8UC4
            QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_ARGB32);       // Format RGBA
            returnImage = qImage.copy();
        }

        break;
        default:{
            const uchar *pSrc = (const uchar*)src.data;                                     // get the pointer of src's data, CV_8UC3
            QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_RGB888);           // create QImage from the given src
            returnImage = qImage.rgbSwapped();                                              // exchange the channel sequence from OpenCV's BGR to RGB
        }

        break;
    }
    return returnImage;
}

void ImageProcess::initCapture(const std::string cameraIndex = "0", const double capWidth = 640,
    const double capHeight = 480) {
#ifdef USE_VIDEO
    cap.open("./video/output.avi");
#else
    cap.open(std::stoi(cameraIndex), cv::CAP_V4L2);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, capWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, capHeight);
#endif

    if(!cap.isOpened()) {
#ifdef _DEBUG
        qDebug() << "Failed to open the camera or video file. ";
#endif  
        m_image_process_running = false;
        runtime_error = "Error, unable to open the camera";
        cap.release();
        emit sendCameraError(runtime_error);
        return; 
    } else {
        m_image_process_running = true;
    }
}


void ImageProcess::readFrame() {

    initCapture(m_camera_index.toStdString());
    
    std::string task_type = m_task_type.toStdString();

    bool is_init = false;

    while(m_image_process_running) {
        cap.read(m_frame);
#ifdef USE_VIDEO
        cv::waitKey(33);
#endif
        if(m_frame.empty()) {
#ifdef USB_VIDEO
            qDebug() << "Frame is empty, will break now";
#endif
            cap.release();
            break;
        }
        else {
            // 模型推理部分
            if(m_nn_running) {
                if(task_type == "MOT") {
                    if(!is_init) {
                        is_init = true;
                        int fps = cap.get(CAP_PROP_FPS);
                        m_infer -> tracker_init(fps, 30);
                    }
                    cv::Mat infer_frame;
                    infer_frame = m_infer->process_image_and_track(m_frame);
                    m_q_frame = MatImageToQImage(infer_frame);
                    emit sendImage(m_q_frame);
                }
                else if(task_type == "VOT") {
                    if(!is_init) {
                        cap.read(m_frame);
                        cv::imshow("Tracker", m_frame);
                        cv::putText(m_frame, "Select target ROI and press ENTER", cv::Point2i(20, 30),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,0,0), 1);
                        cv::waitKey(1);
                        cv::Rect init_bbox = cv::selectROI("Tracker", m_frame); 
                        m_tracker->init_tracker(m_frame, init_bbox);
                    }
                    cv::Mat infer_frame;
                    infer_frame = m_tracker->process_image_and_track(m_frame);
                    m_q_frame = MatImageToQImage(infer_frame);
                    emit sendImage(m_q_frame);
                }    
            }
            else {
                    m_q_frame = MatImageToQImage(m_frame);
                    emit sendImage(m_q_frame);
                }
        }
    }
    cap.release();   
}



void ImageProcess::startCapture() {
    m_image_process_running = true;
}

void ImageProcess::endCapture() {
    QMutexLocker locker(&mutex);
    m_image_process_running = false;
}


 void ImageProcess::initengine(QStringList capturePara, QStringList inferPara) {
    m_camera_index = capturePara[1];
    m_full_network_path = inferPara[1];
    m_task_type = inferPara[2];

    QFileInfo fileInfo(m_full_network_path);
    if(!fileInfo.exists()) {
        m_nn_running = false;
#ifdef _DEBUG
        QString m_error_infer_device = "Error, unable to locate network file.\n";
        qDebug() << m_error_infer_device;
#endif
        runtime_error = "Error, unable to locate  network file.";
        emit sendInferDeviceError(runtime_error);
        return ;
    }

    if(m_task_type == "MOT") {
        m_infer->call_create_infer(m_full_network_path.toStdString());
    }
    else if(m_task_type == "VOT") {
        m_tracker->call_create_tracker(m_full_network_path.toStdString());
    }
    
    emit sendInferDeviceSuccess();
 }


 void ImageProcess::changeNNStatus(){
     m_nn_running = !m_nn_running;
 }
