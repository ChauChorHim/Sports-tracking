# Sports-tracking
This README is a guide to install and build the Sports-tracking project. For interesting project details, check [here](doc/documentation.md).

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#documentation">Documentation</a></li>
  </ol>
</details>

## About The Project
The Sports-tracking project is a code challenge provided by [Swiss Timing](https://swisstiming.com/). It aims to build an efficient and fit-for-purpose solution to process short sports videos with moving athletes and extract information about the athletes. The development environment is maintained using [Docker](https://www.docker.com/), and the project is built using [CMake](https://cmake.org/). [OpenCV](https://opencv.org/) is the primary library used in this project.

![Results](doc/playback.gif)

As this project was completed in a short time, there might be some bugs or issues. Feel free to suggest changes or report issues by opening an issue.



## Getting Started
This section provides instructions for setting up the project on a Windows system. Users on other platforms should follow similar steps with differences in [setting GUI applications communication](https://cuneyt.aliustaoglu.biz/en/running-gui-applications-in-docker-on-windows-linux-mac-hosts/) between the Docker container and the host.

### Prerequisites
To run this project, you need to have the following:
* [Docker](https://www.docker.com/) - Download and install Docker.
* [VcXsrv Windows X Server](https://sourceforge.net/projects/vcxsrv/) - Install the X Server for GUI communication. For more details, read the [documentation](https://cuneyt.aliustaoglu.biz/en/running-gui-applications-in-docker-on-windows-linux-mac-hosts/).
* A short sport video clip for testing.

### Installation
Follow these steps to set up the project:

1. Pull the Docker image with OpenCV version 4.5.4:
```
docker pull adnrv/opencv:latest
```

2. Run the Docker container with mounted volumes:
```
docker run -t -d 
--name sports-tracking 
--mount type=bind,source=$YOUR_PROJECT_PATH$,target=/home/sports-tracking 
-e DISPLAY=$YOUR_IP_ADDRESS$:0.0 adnrv/opencv
```
3. Obtain the pretrained YOLOv5 model and convert it to the ONNX format:
```
git clone https://github.com/ultralytics/yolov5
wget https://github.com/ultralytics/YOLOv5/releases/download/v6.1/YOLOv5s.pt
python export.py --weights YOLOv5s.pt --simplify --opset 12 --include onnx
```

4. Download this repository and build it:
```
git clone https://github.com/ChauChorHim/Sports-tracking.git
mkdir build && cd build
cmake .. && make -j4
```

## Usage

### Configuration file
Edit the configuration file `config/badminton.json` with the following parameters:
* path_to_video: relative path to the video for processing
* path_to_onnx: relative path to the onnx weights of YOLOv5 model
* path_to_save_video: relative path to the processed video and .avi is the only supported format for now
* path_to_save_bbs: relative path to the extracted bounding box information in text format
* diable_gui: diable the real time display function, which requires GUI
* enable_bev: enable the bird's eye view (BEV) alongside with the athlete detection and tracking
* bev_map_size_w: the width of BEV map
* bev_map_size_h: the height of BEV map
* others - input_width_, input_height_, score_threshold_, nms_threshold_, confidence_threshold_ are parameters of YOLOv5. Please check the documentation of YOLOv5 for more details

Edit the configuration file `config/badminton_bev_map.json` to describe the points and connections in the BEV map. For detailed description, please check [here](doc/documentation.md).

### Run the codes
Before running the executable file, ensure that the GUI communication between the Docker container and the host is set up correctly. If GUI communication is not possible, enable the "disable_gui" option in the config file.
```
// Run the main executable file with argument indicating the path to the json file
cd $YOUR_PROJECT_PATH$/bin
main /home/sports-tracking/config/badminton.json 
```

## Documentation
For more detailed documentation and information about the project, refer to [doc/documentation.md](doc/documentation.md).