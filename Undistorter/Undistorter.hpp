#pragma once
#include <opencv2/opencv.hpp>
class Undistorter
{
public:
	Undistorter(cv::FileStorage distCoeffsFile, cv::VideoCapture videoCapture, std::string outputFileName);

	~Undistorter();

	void process();

private:

	cv::Mat map1, map2;
	cv::Size inputSize, outputSize;
	cv::VideoCapture videoCapture;
	int totalFrames;

	void writeFrame(int frameIndex, cv::Mat frame);

	std::string outputPrefix, outputSuffix;
	cv::VideoWriter writer;
	bool writeImages = false;

	void initializeFromCoefficients(cv::FileStorage distCoeffsFile, cv::Size inputSize);
};
