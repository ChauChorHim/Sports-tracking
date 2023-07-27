#include "file_io.h"
#include <cassert>
#include <fstream>

namespace st
{
    void loadVideo(const std::string& path_to_video, cv::VideoCapture& video_capturer)
    {
        cv::VideoCapture tmp(path_to_video);
        assertm(tmp.isOpened(), "Error: unable to open the video\n");
        video_capturer = std::move(tmp);
    } 

    void saveVideo(const std::shared_ptr<std::vector<cv::Mat>>& save_images, const std::string& path_to_save_video)
    {
        // Define the codec for the output video (use XVID for AVI format)
        int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');

        // Get the frame size from the first loaded image
        cv::Size frameSize = (*save_images)[0].size();

        // Create the VideoWriter object
        cv::VideoWriter videoWriter(path_to_save_video, codec, 30.0, frameSize);

        // Check if the VideoWriter was initialized successfully
        assertm(videoWriter.isOpened(), "Error: Unable to open the VideoWriter for writing.");
        
        for (auto& frame : *save_images)
        {
            videoWriter.write(frame);
        }

        videoWriter.release();
    }

    void loadConfig(const std::string& path_to_config, nlohmann::json& config_data)
    {
        std::ifstream f(path_to_config.c_str());
        config_data = nlohmann::json::parse(f); 
    }


    void saveBoundingBox(const std::shared_ptr<std::vector<BoundingBoxInfo>>& p_bbs, const std::string& path_to_save_bbs)
    {
        nlohmann::json output_json;
        for (const auto& bb_info : *p_bbs)
        {
            output_json["data"].push_back({
                {"frame_id", bb_info.frame_id},
                {"human_id", bb_info.human_id},
                {"left_top_x", bb_info.left_top_x},
                {"left_top_y", bb_info.left_top_y},
                {"width", bb_info.width},
                {"height", bb_info.height}
            });
        }

        std::ofstream output_file(path_to_save_bbs);
        output_file << output_json.dump(4);
        output_file.close();
    }

} // namespace st