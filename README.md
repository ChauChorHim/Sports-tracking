# Sports-tracking
This README is used for helping reader to install and build this project. Some interesting description for this project is [here](doc/documentation.md)

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
This project is used for the code challenge provided by [Swiss Timing](https://swisstiming.com/). The task of code challenge is to build a system to process a short sport video with moving athletes and extract the information of the athletes. This project is finished in a short time. THerefore, bugs or issues probably exist and please suggest changes by opening an issues.

### Built With
The development environment is maintained using [Docker](https://www.docker.com/) and the project is built using [CMake](https://cmake.org/). [OpenCV](https://opencv.org/) is the major library used to bootstrap this project.

## Getting Started
This instruction is mainly for users using Windows system. Users using other platform should follow a similar setup with differences in [setting GUI applications communication](https://cuneyt.aliustaoglu.biz/en/running-gui-applications-in-docker-on-windows-linux-mac-hosts/) between docker container and the host.
### Prerequisites
* Download and install Docker
* Dowoload and install [VcXsrv Windows X Server](https://sourceforge.net/projects/vcxsrv/), please read [doc](https://cuneyt.aliustaoglu.biz/en/running-gui-applications-in-docker-on-windows-linux-mac-hosts/
* Prepare a short sport video clip

### Installation
The first step is to obtain the docker container and setup the volumes between container and the host. The OpenCV version uesd in this docker image is 4.5.4.
```
// pull docker image with OpenCV 
docker pull adnrv/opencv:latest

// run docker container
docker run -t -d --name sports-tracking --mount type=bind,source=$YOUR_PROJECT_PATH$,target=/home/sports-tracking -e DISPLAY=$YOUR_IP_ADDRESS$:0.0 adnrv/opencv
```

The second step is to obtain the pretrained YOLOv5 model and transfer it to ONNX model. The environment setup is omitted and you could follow the [official instruction](https://docs.ultralytics.com/quickstart/).
```
// download the YOLOv5 repository
git clone https://github.com/ultralytics/yolov5

// download the official pretrained YOLOv5 model weights 
wget https://github.com/ultralytics/YOLOv5/releases/download/v6.1/YOLOv5s.pt

// convert the PyTorch pretrained model to ONNX model
python export.py --weights YOLOv5s.pt --simplify --opset 12 --include onnx
```

The third step is to download this repository and build it.
```
git clone https://github.com/ChauChorHim/Sports-tracking.git
mkdir build && cd build
cmake .. && make -j4
```

## Usage

### Configuration file
* path_to_video: relative path to the video for processing
* path_to_onnx: relative path to the onnx weights of YOLOv5 model
* path_to_save_video: relative path to the processed video and .avi is the only supported format for now
* path_to_save_bbs: relative path to the extracted bounding box information in text format
* diable_gui: diable the real time display function, which requires GUI
* enable_bev: enable the bird's eye view (BEV) alongside with the athlete detection and tracking
* bev_map_size_w: the width of BEV map
* bev_map_size_h: the height of BEV map
* others - input_width_, input_height_, score_threshold_, nms_threshold_, confidence_threshold_ are parameters of YOLOv5. Please check the documentation of YOLOv5 for more details

### Run the codes
Before running the executable file, please make sure the GUI communication between the docker and the host is OK. If GUI communication is unable to setup, please enable the "disable_gui" in the config file.
```
// Run the main executable file with argument indicating the path to the json file
cd $YOUR_PROJECT_PATH$/bin
main /home/sports-tracking/config/badminton.json 
```

## Documentation
Please refer [doc/documentation.md](doc/documentation.md).