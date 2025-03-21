
#include "node_ostrack.h"

using namespace std;



void AppOStrack::call_create_tracker() {

    string model_addr = "ostrack-256.trt";
    m_tracker_engine = OSTrack::create_tracker(model_addr);
    if(m_tracker_engine == nullptr){
        printf("tracker is nullptr.\n");
        return;
    }
}

void AppOStrack::call_create_tracker(const std::string model_addr) {

    m_tracker_engine = OSTrack::create_tracker(model_addr);
    if(m_tracker_engine == nullptr){
        printf("tracker is nullptr.\n");
        return;
    }
}

void AppOStrack::init_tracker(cv::Mat& frame, cv::Rect init_bbox) {
    // string display_name = "Track";
    // cv::imshow(display_name, frame);
    // cv::putText(frame, "Select target ROI and press ENTER", cv::Point2i(20, 30),
    //             cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,0,0), 1);
    // cv::Rect init_bbox = cv::selectROI(display_name, frame);
    // cv::waitKey(1);
    m_tracker_engine->init(frame, init_bbox);
}


cv::Mat AppOStrack::process_image_and_track(cv::Mat& image) {
    
    cv::Rect bbox;
    // auto start = std::chrono::steady_clock::now();
    bbox = m_tracker_engine->track(image);
    // auto end = std::chrono::steady_clock::now();
    // std::chrono::duration<double> elapsed = end - start;
    // double time = 1000 * elapsed.count();
    // printf("all infer time: %f ms\n", time);
    cv::rectangle(image, bbox, cv::Scalar(0,255,0), 2);
    // cv::imshow("Track", image);
    // cv::waitKey(1);
    return image;
}




