#include "detector.h"
#include "file_io.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>

namespace st
{
    st::HumanDetector::HumanDetector(cv::VideoCapture& video_capturer, const nlohmann::json& config_data) : video_capturer_(video_capturer), end_(false), cur_frame_id_(0) {
        video_capturer_.read(cur_frame_);
        assertm(!cur_frame_.empty(), "Error: unable to read the first frame\n");
        if (true == config_data["enable_bev"].get<bool>())
        {
            auto bev_map_size = cv::Size(config_data["bev_map_size_w"].get<int>(), config_data["bev_map_size_h"].get<int>());
            enable_bev_ = true;
            map_BEV_ = cv::Mat(bev_map_size, CV_8UC3);
            std::string path_to_bev_map = config_data["path_to_project"].get<std::string>() + config_data["path_to_bev_map"].get<std::string>();
            std::ifstream f(path_to_bev_map);
            nlohmann::json map_BEV_json = nlohmann::json::parse(f);

            uint8_t points_num = map_BEV_json["points_num"].get<uint8_t>();
            std::vector<cv::Point> pts;
            for (uint8_t idx = 1; idx <= points_num; ++idx)
            {
                std::ostringstream oss;
                oss << static_cast<int>(idx);
                std::string pt_name = "pt_" + oss.str();
                int x = map_BEV_json[pt_name.c_str()]["x"].get<int>();
                int y = map_BEV_json[pt_name.c_str()]["y"].get<int>();
                pts.push_back({x, y});
            }
            for (const auto& connect: map_BEV_json["connects"])
            {
                cv::line(map_BEV_, pts[connect["head"].get<int>() - 1], pts[connect["tail"].get<int>() - 1], cv::Scalar(255, 255, 255), 2);
            }
            std::cout << "BEV map activated, map_BEV_size: " << map_BEV_.size() << "\n";

            // From the image
            cv::Point2f src[4] = {
                cv::Point2f(545, 435), cv::Point2f(1370, 435), cv::Point2f(365, 1070), cv::Point2f(1565, 1060) 
            };

            // From bird's eye view
            cv::Point2f dst[4] = {
                pts[0], pts[4], pts[17], pts[21]
            };

            // Calculate the perspective transformation matrix
            perspectiveMatrix = cv::getPerspectiveTransform(src, dst);
        }
    }

    // This function is used to detect bounding box information
    cv::Mat st::HumanDetector::detect()
    {
        pre_frame_ = std::move(cur_frame_);
        video_capturer_.read(cur_frame_);
        cur_frame_id_++;

        if (cur_frame_.empty())
        {
            this->end_ = true;
            return cur_frame_;
        }

        this->extractBB();
        
        if (enable_bev_)
        {
            attachBEV();     
        }

        images_.push_back(cur_frame_);

        return cur_frame_;
    }


    void st::HumanDetector::attachBEV()
    {
        std::vector<cv::Point2f> bev_points;
        std::vector<cv::Point2f> src_points;

        std::vector<bool> isOK(next_id_, false);
        for (int idx = bbs_info_.size() - 1; idx >= 0 && src_points.size() <= next_id_; idx--)
        {
            if (false == isOK[bbs_info_[idx].human_id])
            {
                auto& bb_info = bbs_info_[idx];
                src_points.push_back({bb_info.left_top_x + float(bb_info.width),
                    bb_info.left_top_y + float(bb_info.height)});
                isOK[bbs_info_[idx].human_id] = true;
            }
        }
            
        cv::perspectiveTransform(src_points, bev_points, perspectiveMatrix);
        cv::Mat map_BEV_with_human = map_BEV_.clone();
        for (auto& bev_point: bev_points)
        {
            cv::circle(map_BEV_with_human, bev_point, 8, cv::Scalar(0, 255, 0), 3);
        }
        // std::stringstream ss;;
        // ss << std::setfill('0') << std::setw(6) << cur_frame_id_;
        // std::string cur_frame_id_string = ss.str();
        // cv::imwrite("/home/sports-tracking/results/tmp/" + cur_frame_id_string + ".png", map_BEV_with_human);
        cv::resize(map_BEV_with_human, map_BEV_with_human, 
            cv::Size(map_BEV_with_human.cols * cur_frame_.rows / map_BEV_with_human.rows, cur_frame_.rows));
        
        cv::hconcat(cur_frame_, map_BEV_with_human, cur_frame_);
    }

    std::shared_ptr<std::vector<cv::Mat>> st::HumanDetector::getImages()
    {
        return std::make_shared<std::vector<cv::Mat>>(images_);
    }


    std::shared_ptr<std::vector<BoundingBoxInfo>> st::HumanDetector::getBoundingBox()
    {
        return std::make_shared<std::vector<BoundingBoxInfo>>(bbs_info_);
    }

    st::YOLOV5Detector::YOLOV5Detector(cv::VideoCapture& video_capturer_, const nlohmann::json& config_data) : 
        HumanDetector(video_capturer_, config_data), 
        input_width_(config_data["input_width_"].get<float>()),
        input_height_(config_data["input_height_"].get<float>()),
        score_threshold_(config_data["score_threshold_"].get<float>()),
        nms_threshold_(config_data["nms_threshold_"].get<float>()),
        confidence_threshold_(config_data["confidence_threshold_"].get<float>())
    {
        const std::string path_to_project = config_data["path_to_project"].get<std::string>();
        const std::string path_to_onnx = path_to_project + config_data["path_to_onnx"].get<std::string>();
        yolo_net_ = cv::dnn::readNet(path_to_onnx);
    }

    void st::YOLOV5Detector::extractBB()
    {
        std::vector<cv::Mat> detections = pre_process(cur_frame_);
        cv::Mat img = post_process(cur_frame_, detections);
        cur_frame_ = std::move(img);
    }


    std::vector<cv::Mat> st::YOLOV5Detector::pre_process(cv::Mat& input_image)
    {
        // Convert to blob
        cv::Mat blob;
        cv::dnn::blobFromImage(input_image, blob, 1./255., cv::Size(input_width_, input_height_), cv::Scalar(), true, false);

        yolo_net_.setInput(blob);

        // Foward propagate
        std::vector<cv::Mat> outputs;
        yolo_net_.forward(outputs, yolo_net_.getUnconnectedOutLayersNames());

        return outputs;
    }

    cv::Mat st::YOLOV5Detector::post_process(cv::Mat &input_image, std::vector<cv::Mat> &outputs)
    {
        // Initialize vectors to hold respective outputs while unwrapping     detections.
        std::vector<int> class_ids;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;
        // Resizing factor.
        float x_factor = input_image.cols / input_width_;
        float y_factor = input_image.rows / input_height_;
        float *pdata = (float *)outputs[0].data;
        const int net_height = outputs[0].size[1]; // 25200
        const int net_width = 80 + 5;
        // Iterate through 25200 detections.
        for (int i = 0; i < net_height; ++i)
        {
            float confidence = pdata[4];
            // Discard bad detections and continue.
            if (confidence >= confidence_threshold_)
            {
                float * classes_scores = pdata + 5;
                // Create a 1xn Mat and store class scores of n classes. Here n = 1
                cv::Mat scores(1, class_list_.size(), CV_32FC1, classes_scores);
                // Perform minMaxLoc and acquire the index of best class score.
                cv::Point class_id;
                double max_class_score;
                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                // Continue if the class score is above the threshold.
                if (max_class_score > score_threshold_)
                {
                    // Store class ID and confidence in the pre-defined respective vectors.
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);
                    // Center.
                    float cx = pdata[0];
                    float cy = pdata[1];
                    // Box dimension.
                    float w = pdata[2];
                    float h = pdata[3];
                    // Bounding box coordinates.
                    int left = int((cx - 0.5 * w) * x_factor);
                    int top = int((cy - 0.5 * h) * y_factor);
                    int width = int(w * x_factor);
                    int height = int(h * y_factor);
                    // Store good detections in the boxes vector.
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
            // Jump to the next row.
            pdata += net_width;
        }

        // Perform Non-Maximum Suppression and draw predictions.
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, score_threshold_, nms_threshold_, indices);
        for (int idx : indices)
        {
            Human human;
            human.bbox = boxes[idx];
            human.centroid = cv::Point(boxes[idx].x + boxes[idx].width / 2, boxes[idx].y + boxes[idx].height / 2);
            // Match the detected centroid with previous centroids based on proximity (threshold)
            bool matched = false;
            for (auto& prevAthlete : humans_) {
                double distance = cv::norm(human.centroid - prevAthlete.centroid);
                if (distance < 50.0) {
                    human.id = prevAthlete.id; // Assign the same ID
                    human.color = prevAthlete.color;
                    prevAthlete = human; // Update previous athlete info
                    matched = true;
                    break;
                }
            }

            if (!matched) {
                human.id = next_id_++; // Assign a new ID
                human.color = cv::Scalar(std::rand() % 256, std::rand() % 256, std::rand() % 256);
                humans_.push_back(human);
            }

            // Draw bounding boxes and labels on the frame
            cv::rectangle(input_image, boxes[idx], human.color, 2);
            std::string label = "Athlete " + std::to_string(human.id) + " " + cv::format("%.2f", confidences[idx]);
            int baseline;
            cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.75, 2, &baseline);
            cv::putText(input_image, label, cv::Point(boxes[idx].x, boxes[idx].y - 5 - textSize.height),
                        cv::FONT_HERSHEY_SIMPLEX, 0.75, human.color, 2);

            // Save bounding box information as text format
            BoundingBoxInfo bb_info {boxes[idx].tl().x, boxes[idx].tl().y, boxes[idx].width, boxes[idx].height, human.id, cur_frame_id_};
            bbs_info_.push_back(bb_info);
        }
        return input_image;
    }
} // namespace st
