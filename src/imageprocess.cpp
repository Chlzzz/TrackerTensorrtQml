#include "imageprocess.h"


// 解析图片地址
static std::pair<std::string, std::string> parse_image_path(const std::string& image_path) {
    size_t last_slash = image_path.find_last_of("/\\");
    size_t last_dot = image_path.find_last_of('.');

    std::string dir = (last_slash == std::string::npos) ? "" : image_path.substr(0, last_slash);
    std::string ext = (last_dot == std::string::npos) ? ".jpg" : image_path.substr(last_dot);

    return {dir, "*" + ext};
}

ImageProcess::ImageProcess(QObject* parent) : QObject(parent) {
    m_image_process_running = false;
    m_nn_running = false;
    m_full_demo_running = false;
    is_init = false;
//    m_infer = std::make_shared<AppRTdetr>();
//    m_tracker = std::make_shared<AppOStrack>();
}

ImageProcess::~ImageProcess(){

}


/// @brief convert opencv cv::Mat to qt QImage
/// @param src sendImage
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


void ImageProcess::initCapture(const double capWidth = 640,
    const double capHeight = 480) {

//    qDebug() << "Called " << __FUNCTION__;
    // 选择图片序列时，生成图片序列名
    if(m_source_array[0].m_source_type == "DIR") {
        for(int i = 0; i < m_source_array.size(); ++i) {
            std::pair<std::string, std::string> m_pair = parse_image_path(m_source_array[i].m_source);
            std::vector<std::string> image_paths;
            cv::glob(m_pair.first + "/" + m_pair.second, image_paths, false);
            sort(image_paths.begin(), image_paths.end());
            m_dir_array.emplace_back(std::move(image_paths));
        }
    }
    // 其他则初始化capture
    else {
        for(auto& cap : m_cap_array) {
            if (cap.isOpened()) cap.release();
        }

        for(const auto& element : m_source_array) {
            cv::VideoCapture cap;
            if(element.m_source_type == "USB") {
                cap.open(std::stoi(element.m_source), cv::CAP_ANY);
    //            cap.open(std::stoi(element.m_source), cv::CAP_V4L2);
            }
            else {
                cap.open(element.m_source, cv::CAP_ANY);
            }

            if(!cap.isOpened()) {
        #ifdef _DEBUG
                qDebug() << "Failed to open the camera or video file. ";
        #endif
                m_image_process_running = false;
                runtime_error = "Error, unable to open the camera";
                cap.release();
                emit sendCameraError(runtime_error);
                return;
            }

            cap.set(cv::CAP_PROP_FRAME_WIDTH, capWidth);
            cap.set(cv::CAP_PROP_FRAME_HEIGHT, capHeight);
            m_cap_array.emplace_back(std::move(cap));
        }
    }
    m_image_process_running = true;

}

bool ImageProcess::grabFrame(size_t currentid) {

    bool is_empty = false;
    bool is_vector_empty = true;
    m_mat_array.clear();
    if(m_source_array[0].m_source_type == "DIR") {
        cv::Mat m_frame;
        for(int i = 0; i < m_dir_array.size(); ++i) {
            if(currentid >= m_dir_array[i].size()) {
                is_empty = true;
                break;
            }
            m_frame = cv::imread(m_dir_array[i][currentid]);
            m_mat_array.emplace_back(std::move(m_frame));
        }
    }
    else {
        for(int i = 0; i < m_cap_array.size(); ++i) {
            cv::Mat m_frame;
            m_cap_array[i].read(m_frame);
            if(m_frame.empty()) {
                m_cap_array[i].release();
                is_empty = true;
                break;
            }
            m_mat_array.emplace_back(std::move(m_frame));
        }
    }
    return is_empty;
}


void ImageProcess::readFrame() {

//    qDebug() << "Called " << __FUNCTION__;
    initCapture();
    size_t currentid = 0;
    while(m_image_process_running) {
        bool is_empty = false;
        is_empty = grabFrame(currentid++);
        if(is_empty) {
            break;
        }
        // 模型推理部分
//            if(m_nn_running && is_init) {
//                if(m_task_type == "MOT") {
//                    cv::Mat infer_frame;
//                    infer_frame = m_infer->process_image_and_track(m_frame);
//                    m_q_frame = MatImageToQImage(infer_frame);
//                    emit sendImage(m_q_frame, 1);
//                }
//                else if(m_task_type == "VOT") {
//                    cv::Mat infer_frame;
//                    infer_frame = m_tracker->process_image_and_track(m_frame);
//                    m_q_frame = MatImageToQImage(infer_frame);
//                    emit sendImage(m_q_frame, 1);
//                }
//            }
//            else {
//                    m_q_frame = MatImageToQImage(m_frame);
//                    emit sendImage(m_q_frame, 1);
//                }


        for(int i = 0; i < m_mat_array.size(); ++i) {
            m_q_frame = MatImageToQImage(m_mat_array[i]);
            emit sendImage(m_q_frame, i);
        }

        if(m_cap_array.size() == 0) {
            for(auto& cap : m_cap_array) {
                if (cap.isOpened()) cap.release();
            }
        }
    }
}


void ImageProcess::ostrack_init(const QRect &roi) {
    init_bbox = cv::Rect(
        roi.x(),
        roi.y(),
        roi.width(),
        roi.height()
    );
    is_init = true;
}


void ImageProcess::startCapture() {
    m_image_process_running = true;
}

void ImageProcess::endCapture() {
    QMutexLocker locker(&mutex);
    m_image_process_running = false;
}


 void ImageProcess::initengine(std::vector<QStringList>& capturePara, QStringList inferPara) {

     QFileInfo fileInfo(inferPara[1]);
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

    m_full_network_path = inferPara[1].toStdString();
    m_task_type = inferPara[2].toStdString();
    sourcePara sp;
    for(int i = 0; i != capturePara.size(); ++i) {
        sp.m_source_type = capturePara[i][0].toStdString();
        sp.m_source = capturePara[i][1].toStdString();
        m_source_array.emplace_back(std::move(sp));
    }

//    if(m_task_type == "MOT") {
//        m_infer->call_create_infer(m_full_network_path.toStdString());
//    }
//    else if(m_task_type == "VOT") {
//        m_tracker->call_create_tracker(m_full_network_path.toStdString());
//    }
    
    emit sendInferDeviceSuccess();
 }


 void ImageProcess::changeNNStatus(){
    std::string task_type = m_task_type;
//    if(task_type == "MOT") {
//        int fps = cap.get(CAP_PROP_FPS);
//        m_infer -> tracker_init(fps, 30);
//        is_init = true;
//    }
//    else if(task_type == "VOT") {
//        m_tracker->init_tracker(m_frame, init_bbox);
//    }
    m_nn_running = !m_nn_running;
 }



