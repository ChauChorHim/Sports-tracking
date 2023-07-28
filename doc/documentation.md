# Documentation
This documentation describes the solution for ths task provided by Swiss Timing. For more context, please refer to [README.md](../README.md).

## Brief 
The solution for the given task can be summarized as follows:
* Video loading
* Image processing
    * Athlete detection
    * Athlete tracking
    * Bird's eye view map
* Output
    * Real-time display
    * Video playback
    * Extracted information in JSON format 

## Functionality
The implementation of video loading, real-time display, and video playback leverages the OpenCV library. The following sections detail the algorithms used for athlete detection, athlete tracking, bird's-eye view map generation, and extracted information storage in JSON format.

### Athlete detection
Athlete detection, or object detection, forms the core component of this system. For this project, a pretrained YOLOv5 model is utilized to address the object detection problem. The YOLOv5 ONNX model, converted from a PyTorch model, is employed to construct the YOLOv5 network with pretrained weights. The detection process is efficiently facilitated with the help of OpenCV's DNN module [cv::dnn](https://docs.opencv.org/4.x/d2/d58/tutorial_table_of_content_dnn.html).

### Athltete tracking
To track multiple players, each bounding box is associated with a unique label ID. The centroid of each bounding box is recorded, and the distance between two bounding box centroids is computed. If the distance is smaller than a certain threshold, the two bounding boxes are labeled with the same player ID, indicating that they belong to the same player. Conversely, if the distance exceeds the threshold, a new player is identified, and the bounding box is assigned a new player ID.

### Bird's eye view (BEV) map
By utilizing the bounding box information, the approximate coordinates of the athletes' feet can be obtained. A perspective matrix is computed aforehand to map the athlete's coordinates from the camera image to the bird's-eye view plane. The perspective matrix is calculated using [cv::getPerspectiveTransform](https://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html#getperspectivetransform), requiring the coordinates of four badminton court corners in both the camera plane and BEV plane. For example, for the provided badminton court layout, we need to define a BEV map with the specified corner coordinates (points 1, 5, 18, 22).
<!-- ![badminton_bev_map](badminton_bev_map.png) -->
<p align="center">
    <img src="badminton_bev_map.png" alt="drawing" width="700"/>
</p>

### Extracted information in JSON format 
The extracted information is stored in JSON format using a popular JSON library [nlohmann/json](https://github.com/nlohmann/json). The JSON content includes the following data for each valid bounding box: 
* data.frame_id: frame id
* data.human_id: detected human id inside the bounding box
* data.left_top_x: x coordinate of the left top corner of the bounding box
* data.left_top_y: y coordinate of the left top corner of the bounding box
* data.width: width of the bounding box
* data.height: height of the bounding box

<!-- ## Limitations and Future Improvements
### Real-time processing
Even though a real-time display is avaiable in this projcet, it is not a real real-time (25Hz) due to the limitation of hardware. Future improvements could be using a GPU for model reference or using several thread to process the frames simultaneously.

### Skipped frame   
As you can see from the results, the detector fails to detect Viktor (the background player) in some frames. Possible reasons include low detection threshold. In order to tackle this issue, on the one hand, we could increase the detection threshold and on the other hand, we could leverage a Kalman filter to compensate the missing bounding box based on the previous bounding box. -->

## Test video clip
The test video clip used in this project is cropped from in a [Youtube video](https://www.youtube.com/watch?v=TSfvUlU8P28), showcasing a badminton match between two top players, Lin Dan from China and Viktor Axelsen from Denmark, during the 2015 Yonex Open Japan Championship. The RGB camera is stationary, and the video consists of 300 frames with a resolution of 1920x1080.

## Reference
* https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
* https://json.nlohmann.me/
* https://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html#getperspectivetransform
* https://opencv-tutorial.readthedocs.io/en/latest/yolo/yolo.html