
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <opencv2/opencv.hpp>
#include "cvui/cvui.h"
#include "CameraCalibrator/CameraCalibrator.h"
#include "Undistorter/Undistorter.hpp"


int getdir(std::string dir, std::vector<std::string> &files)
{
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) {
		std::cout << "Error opening " << dir << std::endl;
		return 0;
	}

	while ((dirp = readdir(dp)) != NULL) {

		if (!strcmp(dirp->d_name, ".")) continue;
		if (!strcmp(dirp->d_name, "..")) continue;
		if (dirp->d_name[0] == '.') continue;

		files.push_back(std::string(dir + dirp->d_name));
	}
	closedir(dp);
	return 0;
}



int main(int argc, char **argv)
{
	if (argc != 2 && argc != 3 && argc != 4) {
		std::cout << "./DistCorr [pattern_dir/]  " << std::endl;
		std::cout << "./DistCorr [image.jpg] [DistCoeffs.yaml]" << std::endl;
		std::cout << "./DistCorr [image.jpg] [DistCoeffs.yaml] [Display Scale factor]" << std::endl;
		std::cout << "./DistCorr [DistCoeffs.yaml] [inputVid] [OutputVid]" << std::endl;
		return 0;
	}

	CameraCalibrator cameraCalibrator;


	if (argc == 2)
	{
		// ./DistCorr [pattern_dir/] 
		std::string dir = std::string(argv[1]);
		std::vector<std::string> filelist;

		getdir(dir, filelist);

		for (unsigned int i = 0; i < filelist.size(); i++) {
			std::cout << filelist[i] << std::endl;
		}

		std::cout << "Calibration pattern images loaded. \n";

		cv::Mat image = cv::imread(filelist[0], 1);

		cv::Size boardSize(9, 6);
		int detected;
		detected = cameraCalibrator.addChessboardPoints(filelist, boardSize);
		std::cout << detected << " pattern images detected from " << filelist.size() << " loaded images. \n";

		cameraCalibrator.calibrate(image.size());

		cv::Mat outImg = cameraCalibrator.remap(image);
		cv::resize(outImg, outImg, cv::Size(outImg.size().width / 4, outImg.size().height / 4));

		cv::imshow("Undistorted Image", outImg);
		cv::waitKey();

	}

	else
	{
		// ./DistCorr [image.jpg] [DistCoeffs.yaml]
		// ./DistCorr [image.jpg] [DistCoeffs.yaml] [Display Scale factor]
		// ./DistCorr [DistCoeffs.yaml] [inputVid] [OutputVid]
		std::string firstArgument = argv[1];
		auto extension = firstArgument.substr(firstArgument.find_last_of(".") + 1);

		if (extension == "yaml")
		{
			// ./DistCorr [DistCoeffs.yaml] [inputVid] [OutputVid]
			std::string distCoeffsFileName = firstArgument;
			std::string videoFile = argv[2];
			std::string outputFile = argv[3];

			cv::VideoCapture cap(videoFile);

			if (!cap.isOpened())
			{
				std::cout << "Could not load file " << videoFile << std::endl;
				return 0;
			}

			cv::FileStorage distCoeffsFile(distCoeffsFileName, cv::FileStorage::READ);

			if (!distCoeffsFile.isOpened())
			{
				std::cout << "Could not load file " << distCoeffsFileName << std::endl;
				return 0;
			}
			Undistorter undistorter(distCoeffsFile, cap, outputFile);
			undistorter.process();
			cap.release();
		}

		else
		{
			// ./DistCorr [image.jpg] [DistCoeffs.yaml]
			double displayScaleFactor = 1.0;
			if (argc == 4)
			{
				// ./DistCorr [image.jpg] [DistCoeffs.yaml] [Display Scale factor]
				displayScaleFactor = atof(argv[3]);
			}

			std::string fileName = argv[2];
			cameraCalibrator.loadParams(fileName);
			cv::Mat frame = cv::imread(firstArgument);
			cameraCalibrator.setInputData(frame, displayScaleFactor);
			cameraCalibrator.lunchGUI();
		}

	}

	return 0;
}



