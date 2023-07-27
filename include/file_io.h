#ifndef __FILE_IO__
#define __FILE_IO__

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <memory>
#include "json.h"

#define assertm(exp, msg) assert(((void)msg, exp))

struct BoundingBoxInfo
{
    int left_top_x;
    int left_top_y;
    int width;
    int height;
    int human_id;
    uint32_t frame_id;
};

namespace st
{
    void loadVideo(const std::string& path_to_video, cv::VideoCapture& video_capturer);
    void saveFrame(const cv::Mat& cur_frame, const std::string& filename, uint8_t jpeg_quality=-1);
    void saveVideo(const std::shared_ptr<std::vector<cv::Mat>>& save_images, const std::string& path_to_save_video);
    void loadConfig(const std::string& path_to_config, nlohmann::json& config_data);
    void saveBoundingBox(const std::shared_ptr<std::vector<BoundingBoxInfo>>& p_bbs, const std::string& path_to_save_bbs);
} // namespace st


#endif