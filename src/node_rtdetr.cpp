#include "node_rtdetr.h"

using namespace std;

static const char* cocolabels[] = {
    "person", "bicycle", "car", "motorcycle", "airplane",
    "bus", "train", "truck", "boat", "traffic light", "fire hydrant",
    "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse",
    "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
    "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis",
    "snowboard", "sports ball", "kite", "baseball bat", "baseball glove",
    "skateboard", "surfboard", "tennis racket", "bottle", "wine glass",
    "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich",
    "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake",
    "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv",
    "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
    "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
    "scissors", "teddy bear", "hair drier", "toothbrush"
};

// 需要跟踪的类别，可以根据自己需求调整，筛选自己想要跟踪的对象的种类（以下对应COCO数据集类别索引）
static std::vector<int>  trackClasses {0, 1, 2, 3, 5, 7};  // person, bicycle, car, motorcycle, bus, truck

static bool isTrackingClass(int class_id){
	for (auto& c : trackClasses){
		if (class_id == c) return true;
	}
	return false;
}

void AppRTdetr::call_create_infer() {
    INFO("engine initializing");
    m_infer_engine = RTDETR::create_infer(
        "rtdetr-l.FP32.trtmodel",               // engine file
        0,                                      // gpu id
        0.25f,                                  // confidence threshold
        1024,                                   // max objects
        false                                   // preprocess use multi stream
    );
    if(m_infer_engine == nullptr){
        INFOE("Engine is nullptr");
        return;
    }

}

void AppRTdetr::call_create_infer(const std::string model_addr) {
    INFO("engine initializing");
    m_infer_engine = RTDETR::create_infer(
        model_addr,                             // engine file
        0,                                      // gpu id
        0.25f,                                  // confidence threshold
        1024,                                   // max objects
        false                                   // preprocess use multi stream
    );
    if(m_infer_engine == nullptr){
        INFOE("Engine is nullptr");
        return;
    }
}

void AppRTdetr::tracker_init(int frame_rate, int tracker_buffer) {
    m_byte_tracker = std::make_shared<BYTETracker>(frame_rate, tracker_buffer);
}


cv::Mat AppRTdetr::process_signle_image(cv::Mat& image) {
    int ow = image.cols;
    int oh = image.rows;
    auto t0 = iLogger::timestamp_now_float();
    auto boxes = m_infer_engine->commit(image).get();

    for(auto& obj : boxes){
        uint8_t b, g, r;
        tie(b, g, r) = iLogger::random_color(obj.class_label);
        float left   = obj.left * ow;
        float top    = obj.top  * oh;
        float right  = obj.right  * ow;
        float bottom = obj.bottom * oh;
        cv::rectangle(image, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(b, g, r), 5);

        auto name    = cocolabels[obj.class_label];
        auto caption = iLogger::format("%s %.2f", name, obj.confidence);
        int width    = cv::getTextSize(caption, 0, 1, 2, nullptr).width + 10;
        cv::rectangle(image, cv::Point(left-3, top-33), cv::Point(left + width, top), cv::Scalar(b, g, r), -1);
        cv::putText(image, caption, cv::Point(left, top-5), 0, 1, cv::Scalar::all(0), 2, 16);
    }
    auto fee = iLogger::timestamp_now_float() - t0;
    return image;
}


cv::Mat AppRTdetr::process_image_and_track(cv::Mat& image) {
    int ow = image.cols;
    int oh = image.rows;

    auto boxes = m_infer_engine->commit(image).get();

     // output format to bytetrack input format, and filter bbox by class id
     // only maintain objects that isTrackingClass 
    std::vector<Object> objects;
    for(auto& obj : boxes) {
        if (isTrackingClass(obj.class_label)){
            float left   = obj.left * ow;
            float top    = obj.top  * oh;
            float right  = obj.right  * ow;
            float bottom = obj.bottom * oh;
            cv::Rect_<float> rect(left, top, right - left, bottom - top);
            Object object {rect, obj.class_label, obj.confidence};
            objects.push_back(object);
        }
    }

     // process bytetrack
     std::vector<STrack> output_stracks = m_byte_tracker -> update(objects);

     // draw rects
     for (int i = 0; i < output_stracks.size(); ++i) {
        std::vector<float> tlwh = output_stracks[i].tlwh;
        if (tlwh[2] * tlwh[3] > 20) {
            cv::Scalar s = m_byte_tracker -> get_color(output_stracks[i].track_id);
            cv::putText(image, cv::format("%d", output_stracks[i].track_id), cv::Point(tlwh[0], tlwh[1] - 5), 
                    0, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
            cv::rectangle(image, cv::Rect(tlwh[0], tlwh[1], tlwh[2], tlwh[3]), s, 2);
		}
	}

    return image;
}
