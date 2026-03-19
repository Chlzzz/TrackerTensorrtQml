#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <iostream>
#include <string>
#include <memory>
#include <opencv2/opencv.hpp>


namespace VOT {

    class Tracker{
    public:
        virtual void init(cv::Mat &z_img, cv::Rect &init_bbox) = 0;
        virtual cv::Rect track(cv::Mat& x_img) = 0;
    };

    std::vector<float> gen_window(int sz);
    void cropSubImg(const cv::Mat& img, cv::Mat& dst, float area_factor, int model_sz, float& resize_factor);

    std::shared_ptr<Tracker> create_tracker(const std::string &engine_path, int gpuid = 0);
        
}


#endif //TRACKER_HPP
