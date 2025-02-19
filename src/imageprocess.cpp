#include "imageprocess.h"

ImageProcess::ImageProcess(QObject* parent) : QObject(parent) {
    m_image_process_running = false;
    m_yolo_running = false;
    m_full_demo_running = false;
    //infer = new CPUInfer();
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

void ImageProcess::initCapture(const int cameraIndex = 0, const double capWidth = 640,
    const double capHeight = 480) {
#ifdef USE_VIDEO
    cap.open("../video/demo.mp4");
#else
    cap.open(cameraIndex, cv::CAP_DSHOW);
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
    initCapture(m_camera_index, m_capture_width, m_capture_height);
    while(m_image_process_running) {
        cap.read(m_frame);
#ifdef USE_VIDEO
        cv::waitKey(33);
#endif
        if(m_frame.empty()) {
#ifdef USB_VIDEO
            qDebug() << "Frame is empty, will break now";
#endif      
            break;
        } 
        else {
            if(m_yolo_running) {
            //     cv::Mat yolo_frame = infer->startInfer(m_frame);
            //     m_q_image = MatImageToQImage(yolo_frame);
            //     emit sendImage(m_q_image);
            // } else {
            //     m_q_image = MatImageToQImage(m_frame);
            //     emit sendImage(m_q_image);
            }
        }
    }
    cap.release();
}