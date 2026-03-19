#include "Tracker.hpp"
#include "OSTrack.hpp"
#include "BAT.hpp"

#include <vector>
#include "infer/trt_infer.hpp"
#include "common/ilogger.hpp"

namespace VOT {

    std::vector<float> Tracker::gen_window(int sz) {
        std::vector<float> hann1d(sz);
        std::vector<float> hann2d(sz * sz);
        for (int i = 1; i < sz + 1; ++i) {
            float w = 0.5f - 0.5f * std::cos(2 * 3.1415926535898f * i / float(sz+1) );
            hann1d[i-1] = w;
        }
        for (int i = 0; i < sz; ++i) {
            for (int j = 0; j < sz; ++j) {
                hann2d[i*sz + j] = hann1d[i] * hann1d[j];
            }
        }
        return hann2d;
    }

    void Tracker::cropSubImg(const cv::Mat& img, cv::Mat& dst, float area_factor, int model_sz, float& resize_factor) const {
            cv::Mat img_patch_roi;  // 填充不缩放
            float cx = target_bbox_.x + 0.5f * target_bbox_.width;
            float cy = target_bbox_.y + 0.5f * target_bbox_.height;
            int crop_sz = std::ceil(std::sqrt(target_bbox_.width * target_bbox_.height) * area_factor);

            // 计算出剪裁边框的左上角和右下角
            int crop_x1 = std::round(cx - crop_sz * 0.5f);
            int crop_y1= std::round(cy - crop_sz * 0.5f);
            int crop_x2 = crop_x1 + crop_sz;
            int crop_y2 = crop_y1 + crop_sz;

            // 边界部分要填充的像素
            int left_pad = std::max(0, -crop_x1);
            int top_pad = std::max(0, -crop_y1);
            int right_pad = std::max(0, crop_x2 - img.cols + 1);
            int bottom_pad = std::max(0, crop_y2 - img.rows + 1);

            // 填充之后的坐标(即要裁切的ROI)
            cv::Rect crop_roi(crop_x1 + left_pad, crop_y1 + top_pad, crop_x2 - crop_x1, crop_y2 - crop_y1);
            if (left_pad > 0 || top_pad > 0 || right_pad > 0 || bottom_pad > 0) {
                // 填充像素
                cv::Mat pad_img;
                cv::Scalar value{114, 114, 114};  // 填充灰色
                cv::copyMakeBorder(img, pad_img, top_pad, bottom_pad,
                                   left_pad, right_pad, cv::BORDER_CONSTANT, value);
                img_patch_roi = pad_img(crop_roi);
            }
            else{
                img_patch_roi = img(crop_roi);
            }

            // 缩放
            cv::resize(img_patch_roi, dst, cv::Size(model_sz, model_sz));

            resize_factor = float(model_sz) / float(crop_sz);
    }

    std::shared_ptr<Tracker> Tracker::create_tracker(const std::string &engine_path, int gpuid = 0) {
        if(engine_path.find("OSTrack") != std::string::npos){
            std::shared_ptr<TrackerImpl> instance(new TrackerImpl{});
            if(!instance->startup(engine_path, gpuid))
                instance.reset();
            return instance;
        }
        else if(engine_path.find("BAT") != std::string::npos){
            std::shared_ptr<TrackerImpl> instance(new TrackerImpl{});
            if(!instance->startup(engine_path, gpuid))
                instance.reset();
            return instance;
        }
        else{
            LOGE("unsupported tracker type, only support OSTrack and BAT");
            return nullptr;
        }
    }
    
    
}