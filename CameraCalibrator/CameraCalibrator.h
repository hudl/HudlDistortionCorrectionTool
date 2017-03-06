#pragma once

#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../cvui/cvui.h"

class CameraCalibrator {

    std::vector<std::vector<cv::Point3f> > objectPoints;
    std::vector<std::vector<cv::Point2f> > imagePoints;

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat map1, map2;
    double step;
    double fx, fy;
    double cx, cy;

    cv::Mat input_image;
    double distCoeff1, distCoeff2, distCoeff3, distCoeff4, distCoeff5, distCoeff6, distCoeff7, distCoeff8;
    double scale, projectionScale;



public:
    CameraCalibrator() : scale(1.0), projectionScale(1.65), step(0.001) {};


    int addChessboardPoints(const std::vector<std::string>& filelist, cv::Size & boardSize);
    double calibrate(cv::Size imageSize);
    cv::Mat remap(const cv::Mat &image);
    cv::Mat remap2(const cv::Mat &image);
    void lunchGUI();
    cv::Mat getCameraMatrix() { return cameraMatrix; }
    cv::Mat getDistCoeffs() { return distCoeffs; }

    void saveParams(cv::Mat camMat, cv::Mat DistCoeffs, double scale, double projectionScale);
    void loadParams(std::string fileName);

    void setInputData(cv::Mat inputImg, double dsf);

    double disp_scale_factor;


};



