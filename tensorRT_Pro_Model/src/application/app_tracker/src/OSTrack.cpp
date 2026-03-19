
#include "OSTrack.hpp"
#include <vector>
#include "infer/trt_infer.hpp"
#include "common/ilogger.hpp"

namespace OSTrack {

    using namespace std;

    
    ~TrackerImpl() = default;

    bool TrackerImpl::startup(const std::string &engine_path, int gpuid) {
        gpu_ = gpuid;
        TRT::set_device(gpuid);

        infer_model_ = TRT::load_infer(engine_path);
        if(infer_model_ == nullptr){
            INFOE("Load model failed: %s", engine_path.c_str());
            return false;
        }
        stream_ = infer_model_->get_stream();
        infer_model_->print();

        return true;
    }

    void TrackerImpl::init(cv::Mat &z_img, cv::Rect &init_bbox) override {
        target_bbox_ = init_bbox;
        zin_ = infer_model_->input(0);
        int z_in_h = zin_->shape(2);
        if(z_in_h == 192){
            search_factor_ = 5.0f;
            template_size_ = 192;
            search_size_ = 384;
            feat_sz_ = 24;
        }
        han_window_ = gen_window(feat_sz_);

        cv::Mat z_patch;
        float resize_factor = 1.f;
        cropSubImg(z_img, z_patch, template_factor_, template_size_, resize_factor);

        float m[] = {0.406, 0.456, 0.485};
        float std[]  = {0.225, 0.224, 0.229};
        zin_->set_norm_mat_invert(0, z_patch, m, std);
    }

    cv::Rect TrackerImpl::track(cv::Mat& x_img) override {
        cv::Mat x_patch;
        float resize_factor = 1.f;
        cropSubImg(x_img, x_patch, search_factor_, search_size_, resize_factor);

        // 绑定输入
        xin_ = infer_model_->input(1);
        float m[] = {0.406, 0.456, 0.485};
        float std[]  = {0.225, 0.224, 0.229};
        xin_->set_norm_mat_invert(0, x_patch, m, std);

        // 绑定输出
        score_map_ = infer_model_->output(1);
        size_map_ = infer_model_->output(2);
        offset_map_ = infer_model_->output(0);

        // 推理
        infer_model_->forward();

        // 后处理，计算bbox
        auto* score_ptr = score_map_->cpu<float>();
        auto* size_ptr = size_map_->cpu<float>();
        auto* offset_ptr = offset_map_->cpu<float>();

        float max_value = han_window_[0] * score_ptr[0];
        int max_idx = 0;
        float tmp_score = 0.f;
        for (int i = 0; i < score_map_->numel(); i++) {
            tmp_score = han_window_[i] * score_ptr[i];
            if (tmp_score > max_value) {
                max_idx = i;
                max_value = tmp_score;
            }
        }
        int max_idx_y = max_idx / score_map_->shape(2);
        int max_idx_x = max_idx % score_map_->shape(2);

        float cx = float(max_idx_x + offset_ptr[max_idx_y * offset_map_->width() + max_idx_x]) / feat_sz_;
        float cy = float(max_idx_y + offset_ptr[offset_map_->width() * offset_map_->height() + max_idx_y * offset_map_->width() + max_idx_x]) / feat_sz_;

        float w = size_ptr[max_idx_y * size_map_->width() + max_idx_x];
        float h = size_ptr[size_map_->width() * size_map_->height() + max_idx_y * size_map_->width() + max_idx_x];

        cx = cx * search_size_ / resize_factor;
        cy = cy * search_size_ / resize_factor;
        w = w * search_size_ / resize_factor;
        h = h * search_size_ / resize_factor;

        float cx_prev = target_bbox_.x + 0.5f * target_bbox_.width;
        float cy_prev = target_bbox_.y + 0.5f * target_bbox_.height;
        float half_side = 0.5f * search_size_ / resize_factor;
        float cx_real = cx + (cx_prev - half_side);
        float cy_real = cy + (cy_prev - half_side);

        target_bbox_.x = std::round(cx_real - 0.5f * w);
        target_bbox_.y = std::round(cy_real - 0.5f * h);
        target_bbox_.width = std::round(w);
        target_bbox_.height = std::round(h);

        target_bbox_.x = std::max(0, std::min(x_img.cols - 5, target_bbox_.x));
        target_bbox_.y = std::max(0, std::min(x_img.rows - 5, target_bbox_.y));
        target_bbox_.width = std::max(5, std::min(x_img.cols, target_bbox_.width));
        target_bbox_.height = std::max(5, std::min(x_img.rows, target_bbox_.height));

        return target_bbox_;
    } 


}