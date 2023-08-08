#include "file_io.h"
#include "detector.h"
#include "json.h"
#include "timer.h"

#include <cstring>

int main(int argc, char** argv)
{
    assertm(argc == 2, "Error: the number of arguments should be 2\n");    
    std::string path_to_config(argv[1]);
    nlohmann::json config_data;
    st::loadConfig(path_to_config, config_data);

    auto path_to_project = config_data["path_to_project"].get<std::string>();
    auto path_to_video = path_to_project + config_data["path_to_video"].get<std::string>();

    cv::VideoCapture video_capturer;
    st::loadVideo(path_to_video, video_capturer);

    auto yoloDetector = st::YOLOV5Detector(video_capturer, config_data);

    {
        st::ModuleTimer timer = st::ModuleTimer("while function");
        while (1)
        {
            auto cur_frame = yoloDetector.HumanDetector::detect();
            if(true == yoloDetector.isEnd())
                break;
            if (false == config_data["disable_gui"].get<bool>())
            {
                cv::namedWindow("Real-time Display", cv::WINDOW_NORMAL);
                cv::imshow("Real-time Display", cur_frame);
                cv::waitKey(50);   
            }
        }
    }

    auto psave_images = yoloDetector.getImages();
    auto path_to_save_video = path_to_project + config_data["path_to_save_video"].get<std::string>();
    st::saveVideo(psave_images, path_to_save_video);

    auto psave_bbs = yoloDetector.getBoundingBox();
    auto path_to_save_bbs = path_to_project + config_data["path_to_save_bbs"].get<std::string>();
    st::saveBoundingBox(psave_bbs, path_to_save_bbs);

    return 0;
}