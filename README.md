
# Distortion Correction tool

This is a tool for generating, tweaking and applying lens distortion correction parameters on videos captured with wide angle/ fisheye lenses. 

This tool is based on the OpenCV library https://github.com/opencv/opencv and the CVUI lib https://github.com/Dovyski/cvui


## Mode 1: Generate camera parameters:

To generate the camera paramaters (camera instrinsic matrix and lens destortion coefficents), take (10 to 30) photos of the calibration pattern in different poses. http://docs.opencv.org/2.4/_downloads/pattern.png

Usage : `./DistortionCorrection imges_dir`

Example : ` /DistortionCorrection "data/patternImages/" `

### Parameters 
`images_dir` directory of calibration pattern images.

### Outputs
`CamaraParams.yaml`  Computed camera Parameters.


## Mode 2: Tweak camera parameters:

This mode will lunch an intaractive GUI that will allow you to tweak the camera parameters in the initial `.yaml` file from Mode 1 for more vidually appealing results. Once you are done with tweaking, press the space, escape or enter key to save the outputs.

Usage :` ./DistortionCorrection  one_image InCameraParams.yaml DisplayScaleFactor`

Example 1 : ` /DistortionCorrection "ExampleData/image.jpg" "data/DistCoeffs.yaml" 2.5 `

Example 2 : `/DistortionCorrection "ExampleData/image.jpg" "data/DistCoeffs.yaml"`

### Parameters:

`InCamaraParams.yaml`- input camera Parameters.

`one_image` - a single frame of the video to be corrected.

`DisplayScaleFactor` - optional - display scale factor (use when frame size is too large to fit in screen).


### Outputs:

`CamaraParams.yaml` - Tweaked camera Parameters.

`CorrectedImage.jpg` - Distortion Corrected image.


## Mode 3: Apply parameters to a video:


This script takes an input video and output directory and a `.yaml` file, and produces distrortion-corrected images.

Usage : `./DistortionCorrection CamaraParams.yaml inputVid Output `

Example 1: ` ./DistortionCorrection "CamaraParams.yaml" "data/in_video.mp4" "data/out_video.mp4"`

Example 2: ` ./DistortionCorrection "CamaraParams.yaml" "data/in_video.mp4" "data/out_images/*.jpg"`

### Parametres:

`CamaraParams.yaml` -  camera Parameters file (generted by mode 1 or 2).

`inputVid` - Input video to be undistorted.

`Output` - If this is an `.mp4` file, it'll write a video stream. If it uses a pattern like `directory/*.png` or `directory/*.jpg`, it'll write a series of images. 


![tool](https://cloud.githubusercontent.com/assets/6253920/23218047/09b3ad3c-f913-11e6-8977-d7e10e4f34eb.jpg)
