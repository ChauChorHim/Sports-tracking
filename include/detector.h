#ifndef __DETECTOR__
#define __DETECTOR__

#include "opencv2/opencv.hpp"
#include <unordered_map>
#include <vector>
#include <memory>

#include "json.h"
#include "file_io.h"

namespace st
{
    struct Human 
    {
        cv::Rect bbox;
        cv::Point centroid;
        int id;
        cv::Scalar color;
    };

    class HumanDetector
    {
        public:
            HumanDetector(cv::VideoCapture& video_capturer, const nlohmann::json& config_data);
            virtual ~HumanDetector() {}
            cv::Mat detect();
            const bool isEnd() { return end_; }
            std::shared_ptr<std::vector<cv::Mat>> getImages();
            std::shared_ptr<std::vector<BoundingBoxInfo>> getBoundingBox();
        protected:
            cv::Mat cur_frame_;
            cv::Mat pre_frame_;
            std::vector<Human> humans_;
            uint8_t next_id_ = 1;
            std::vector<BoundingBoxInfo> bbs_info_;
            uint32_t cur_frame_id_;
            
        private:
            virtual void extractBB() = 0;
            void attachBEV();
            cv::VideoCapture video_capturer_;
            bool end_;
            std::vector<cv::Mat> images_;
            cv::Mat map_BEV_;
            bool enable_bev_ = false;
            cv::Mat perspectiveMatrix;
    };

    // Reference: https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
    class YOLOV5Detector: public HumanDetector
    {
        public:
            YOLOV5Detector(cv::VideoCapture& video_capturer_, const nlohmann::json& config_data);
        private:
            std::vector<std::string> class_list_ {"person"};
            cv::dnn::Net yolo_net_;

            // Constants.
            const float input_width_;
            const float input_height_;
            const float score_threshold_;
            const float nms_threshold_;
            const float confidence_threshold_;

            void extractBB();
            std::vector<cv::Mat> pre_process(cv::Mat& input_image);
            cv::Mat post_process(cv::Mat &input_image, std::vector<cv::Mat> &outputs);

    };

} // namespace st


#endif