#ifndef _APP_OSTRACL_H_
#define _APP_OSTRACK_H_

#include <iostream>
#include <fstream>
#include <boost/format.hpp>

#include "common/ilogger.hpp"
#include "app_ostrack/OSTrack.hpp"

class AppOStrack {
    public:
        AppOStrack() {m_tracker_engine = std::shared_ptr<OSTrack::Tracker>();};
        ~AppOStrack() {};
    
        void call_create_tracker();
        void call_create_tracker(const std::string model_addr);
    

        void init_tracker(cv::Mat& image, cv::Rect init_bbox);
        cv::Mat process_image_and_track(cv::Mat& image);

        
    private:
        std::shared_ptr<OSTrack::Tracker> m_tracker_engine;
    };



#endif  // _APP_OSTRACK_H_