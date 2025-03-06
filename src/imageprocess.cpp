#include "imageprocess.h"

ImageProcess::ImageProcess(QObject* parent) : QObject(parent) {
    m_image_process_running = false;
    m_nn_running = false;
    m_full_demo_running = false;
    m_infer = std::make_shared<AppRTdetr>();
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
    cap.open(0, cv::CAP_V4L2);
    //cap = cv::VideoCapture(0);
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
    initCapture();
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
            if(m_nn_running) {
                 cv::Mat infer_frame = m_infer->process_signle_image(m_frame);
                 m_q_frame = MatImageToQImage(infer_frame);
                 emit sendImage(m_q_frame);
            } else {
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

 void ImageProcess::checkInferParameter(QVector<int> capturePara, QStringList inferPara) {
 #ifdef _DEBUG
     qDebug() << "Received signal sendToThread, checking parameters now, in function " << __FUNCTION__;
 #endif
     m_camera_index = capturePara[0];
     m_capture_width =capturePara[1];
     m_capture_height = capturePara[2];
    
     QString m_infer_device = inferPara[0];
     QString m_network_dir = inferPara[1];
     QString m_model_type = inferPara[2];
   
     QStringList switchList;
     switchList << "fusion" << "detection" << "tracking";
     // got to check the file
     // I hate if... so switch case is used.
     if(m_infer_device == "Intel CPU"){
         switch(switchList.indexOf(m_model_type)){
             case 1:
                 m_full_network_path = m_network_dir + "/" + "yolox_tiny.xml";
 #ifdef _DEBUG
                 qDebug() << "fusion, file path: " << m_full_network_path;
 #endif
                 break;
             case 2:
                 m_full_network_path = m_network_dir + "/" + "yolox_s.xml";
 #ifdef _DEBUG
                 qDebug() << "detection, file path: " << m_full_network_path;
 #endif
                 break;
             case 3:
                 m_full_network_path = m_network_dir + "/" + "yolox_m.xml";
 #ifdef _DEBUG
                 qDebug() << "tracking, file path: " << m_full_network_path;
 #endif
                 break;
             default:
                 m_full_network_path = m_network_dir + "/" + "yolox_nano.xml";
 #ifdef _DEBUG
                 qDebug() << "default method is fusion,  file path: " << m_full_network_path;
 #endif
                 break;

              // check if the file exsits...
         }
         QFileInfo fileInfo(m_full_network_path);
         if(!fileInfo.exists()) {
             m_nn_running = false;
 #ifdef _DEBUG
             QString m_error_infer_device = "Error, unable to locate network file.\n";
             qDebug() << m_error_infer_device;
 #endif
             runtime_error = "Error, unable to locate yolo network file.";
             emit sendInferDeviceError(runtime_error);
             return ;
         }
         else {
             // If nothing happened, our camera and engine will init here...
             //infer->initEngine(m_full_network_path.toStdString(), "CPU");
             emit sendInferDeviceSuccess();
         }
         // else {

         //     // do something
         // }
     }
 }


 void ImageProcess::initengine() {
    m_infer -> call_create_infer();
    emit sendInferDeviceSuccess();
 }


 void ImageProcess::changeNNStatus(){
     m_nn_running = !m_nn_running;
 }
