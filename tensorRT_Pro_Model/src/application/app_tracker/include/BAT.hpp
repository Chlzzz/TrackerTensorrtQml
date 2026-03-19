#include "Tracker.hpp"

namespace BAT{
    
    class TrackerImpl : public VOT::Tracker{
    public:
        ~TrackerImpl() = default;
        
        bool startup(const std::string &engine_path, int gpuid);

        void init(cv::Mat &z_img, cv::Rect &init_bbox) override;

        cv::Rect track(cv::Mat& x_img) override;
    
    private:
        float template_factor_ = 2.0f;
        float search_factor_ = 4.0f; // 5.0f
        int template_size_ = 128; //192
        int search_size_ = 256; // 384
        int feat_sz_ = 16; // 24
        std::vector<float> han_window_;
        cv::Rect target_bbox_; // 目标框

        std::shared_ptr<TRT::Infer> infer_model_;
        std::shared_ptr<TRT::Tensor> zin_;
        std::shared_ptr<TRT::Tensor> xin_;
        std::shared_ptr<TRT::Tensor> score_map_;
        std::shared_ptr<TRT::Tensor> size_map_;
        std::shared_ptr<TRT::Tensor> offset_map_;
        TRT::CUStream stream_ = nullptr;
        int gpu_ = 0;
    }

}