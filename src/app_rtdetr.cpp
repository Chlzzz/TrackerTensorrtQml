#include "app_rtdetr.h"

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

cv::Mat AppRTdetr::process_signle_image(cv::Mat& image) {
    int ow = image.cols;
    int oh = image.rows;

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
    return image;
}

