#ifndef _APP_RTDETR_H_
#define _APP_RTDETR_H_


#include <memory>
#include "builder/trt_builder.hpp"
#include "infer/trt_infer.hpp"
#include "common/ilogger.hpp"
#include "app_rtdetr/rtdetr.hpp"
#include "tools/zmq_remote_show.hpp"
#include "BYTETracker.h"


class AppRTdetr {
public:
    AppRTdetr() {m_infer_engine = std::shared_ptr<RTDETR::Infer>();};
    ~AppRTdetr() {};

    void call_create_infer();
    void call_create_infer(const std::string model_addr);

    void tracker_init(int frame_rate, int tacker_buffer);

    cv::Mat process_signle_image(cv::Mat& image);
    cv::Mat process_image_and_track(cv::Mat& image);
    
private:
    std::shared_ptr<RTDETR::Infer> m_infer_engine;
    std::shared_ptr<BYTETracker> m_byte_tracker;
};


#endif //  _APP_RTDETR_H_
