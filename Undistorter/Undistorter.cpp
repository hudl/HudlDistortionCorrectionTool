#include "Undistorter.hpp"
#include <iomanip>


Undistorter::Undistorter(cv::FileStorage distCoeffsFile, cv::VideoCapture videoCapture, std::string outputFileName)
{
	this->videoCapture = videoCapture;

	auto inputW = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
	auto inputH = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
	inputSize = cv::Size(inputW, inputH);
	initializeFromCoefficients(distCoeffsFile, inputSize);

	totalFrames = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);

	auto extension = outputFileName.substr(outputFileName.find_last_of(".") + 1);
	if (extension == "mp4")
	{
		double frameRate = videoCapture.get(CV_CAP_PROP_FPS);
		writer = cv::VideoWriter(outputFileName, CV_FOURCC('X', '2', '6', '4'), frameRate, outputSize);
	}
	else
	{
		writeImages = true;
		auto index = outputFileName.find_last_of("*");
		outputPrefix = outputFileName.substr(0, index);
		outputSuffix = outputFileName.substr(index + 1);
	}
}

void Undistorter::writeFrame(int frameIndex, cv::Mat frame)
{
	if (writeImages)
	{
		std::stringstream fileName;
		fileName << outputPrefix << std::setw(7) << std::setfill('0') << frameIndex << outputSuffix;
		cv::imwrite(fileName.str(), frame);
	}
	else
	{
		writer.write(frame);
	}
}

Undistorter::~Undistorter()
{
	if (!writeImages)
	{
		writer.release();
	}
}

void Undistorter::initializeFromCoefficients(cv::FileStorage distCoeffsFile, cv::Size inputSize)
{
	cv::Mat cameraMatrix, distCoeffs;
	distCoeffsFile["cameraMatrix"] >> cameraMatrix;
	distCoeffsFile["distCoeffs"] >> distCoeffs;
	double scale = distCoeffsFile["Scale"];

	double sf = distCoeffsFile["ProjectionScale"];
	double fx, fy, cx, cy;
	fx = distCoeffsFile["fx"];
	fy = distCoeffsFile["fy"];
	cx = distCoeffsFile["cx"];
	cy = distCoeffsFile["cy"];
	distCoeffsFile.release();

	cv::Matx33d newK(scale *  fx, 0, scale * cx,
		0, scale * fy, scale * cy,
		0, 0, 1);

	cv::Size scaledInputSize(inputSize.width * sf, inputSize.height * sf);
	outputSize = scaledInputSize;
	cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), newK, scaledInputSize, CV_32F, map1, map2);
}


void Undistorter::process()
{
	std::cout << "Processing with CPU" << std::endl;

	cv::Mat inputFrame, remapped, outputFrame;
	int i = 0;
	while (true)
	{
		videoCapture >> inputFrame;

		if (inputFrame.empty())
		{
			printf("End of data \n");
			break;
		}

		cv::remap(inputFrame, remapped, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());

		writeFrame(i, remapped);
		i++;
	}
}