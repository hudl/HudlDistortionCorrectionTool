#include "CameraCalibrator.h"
#include <thread>

class ImageProcessor {
public:
    ImageProcessor(cv::Mat inputImage, CameraCalibrator *cameraCalibrator);
    void needsUpdate();
    void finish();
    ~ImageProcessor();

private:
    cv::Mat _inputImage, processedImage;
    std::thread _processThread;
    CameraCalibrator *_cameraCalibrator;
    int _shouldFinish;
    int _needsUpdate;
    void processImage();
};

int CameraCalibrator::addChessboardPoints(
    const std::vector<std::string>& filelist, cv::Size & boardSize) {

    std::vector<cv::Point2f> points2D;
    std::vector<cv::Point3f> points3D;


    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {

            points3D.push_back(cv::Point3f(i, j, 0.0f));
        }
    }
    cv::Mat image, image2;
    int detected = 0;
    for (int i = 0; i < filelist.size(); i++) {

        image = cv::imread(filelist[i], 0);
        image2 = cv::imread(filelist[i], 1);

        bool found = cv::findChessboardCorners(
            image, boardSize, points2D);

        if (points2D.size() == boardSize.area()) {

            imagePoints.push_back(points2D);
            objectPoints.push_back(points3D);
            detected++;
            cv::drawChessboardCorners(image2, boardSize, points2D, found);
            cv::imshow("Corners on Chessboard", image2);
            cv::waitKey(2);
        }

    }

    cv::destroyWindow("Corners on Chessboard");
    return detected;
}


double CameraCalibrator::calibrate(cv::Size imageSize)
{
    std::vector<cv::Mat> rvecs, tvecs;
    return calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_RATIONAL_MODEL);
}

cv::Mat CameraCalibrator::remap(const cv::Mat &image)
{
    cv::Mat undistorted;
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cv::Mat(), image.size(), CV_32F, map1, map2);
    cv::remap(image, undistorted, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT);

    fx = cameraMatrix.at<double>(0, 0);
    fy = cameraMatrix.at<double>(1, 1);
    cx = cameraMatrix.at<double>(0, 2);
    cy = cameraMatrix.at<double>(1, 2);

    saveParams(cameraMatrix, distCoeffs, scale, projectionScale);
    return undistorted;
}

cv::Mat CameraCalibrator::remap2(const cv::Mat &image)
{
    cv::Mat undistorted;
    distCoeffs.at<double>(0, 0) = distCoeff1;
    distCoeffs.at<double>(0, 1) = distCoeff2;
    distCoeffs.at<double>(0, 2) = distCoeff3;
    distCoeffs.at<double>(0, 3) = distCoeff4;
    distCoeffs.at<double>(0, 4) = distCoeff5;
    distCoeffs.at<double>(0, 5) = distCoeff6;
    distCoeffs.at<double>(0, 6) = distCoeff7;
    distCoeffs.at<double>(0, 7) = distCoeff8;

    cv::Matx33d newK(scale *  fx, 0, scale * cx,
        0, scale * fy, scale * cy,
        0, 0, 1);

    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), newK, cv::Size(image.size().width*projectionScale, image.size().height*projectionScale), CV_32F, map1, map2);
    cv::remap(image, undistorted, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT);

    saveParams(cameraMatrix, distCoeffs, scale, projectionScale);
    return undistorted;
}

void CameraCalibrator::saveParams(cv::Mat camMat, cv::Mat distCoeffs, double scale, double projectionScale)
{

    cv::FileStorage file("CameraParams.yaml", cv::FileStorage::WRITE);

    file << "cameraMatrix" << camMat << "distCoeffs" << distCoeffs;

    file << "Scale" << scale;
    file << "ProjectionScale" << projectionScale;

    file << "fx" << fx;
    file << "fy" << fy;
    file << "cx" << cx;
    file << "cy" << cy;

    file.release();

}

void CameraCalibrator::loadParams(std::string fileName)
{
    cv::FileStorage file(fileName, cv::FileStorage::READ);

    file["cameraMatrix"] >> cameraMatrix;
    file["distCoeffs"] >> distCoeffs;

    scale = file["Scale"];

    fx = file["fx"];
    fy = file["fy"];
    cx = file["cx"];
    cy = file["cy"];

    distCoeff1 = distCoeffs.at<double>(0, 0);
    distCoeff2 = distCoeffs.at<double>(0, 1);
    distCoeff3 = distCoeffs.at<double>(0, 2);
    distCoeff4 = distCoeffs.at<double>(0, 3);
    distCoeff5 = distCoeffs.at<double>(0, 4);
    distCoeff6 = distCoeffs.at<double>(0, 5);
    distCoeff7 = distCoeffs.at<double>(0, 6);
    distCoeff8 = distCoeffs.at<double>(0, 7);
}

void CameraCalibrator::setInputData(cv::Mat inputImg, double dsf)
{
    input_image = inputImg.clone();
    disp_scale_factor = dsf;

}


void CameraCalibrator::lunchGUI()
{

    cv::Mat frame = cv::Mat(300, 800, CV_8UC3);

    cv::namedWindow("Distortion Controler");
    cvui::init("Distortion Controler");
    ImageProcessor *imageProcessor = new ImageProcessor(input_image, this);

    while (true) {
        frame = cv::Scalar(49, 52, 49);

        cvui::text(frame, 90, 20, "Camera Parameters");
        cvui::text(frame, 400, 20, "Distortion Coefficients");

        cvui::text(frame, 115, 53, "Zoom");
        cvui::text(frame, 100, 103, "X Shift");
        cvui::text(frame, 100, 153, "Y Shift");
        cvui::text(frame, 50, 203, "X Focal Length");
        cvui::text(frame, 50, 253, "Y Focal Length");

        cvui::counter(frame, 160, 50, &scale, 0.01, "%.2f");
        cvui::counter(frame, 160, 100, &cx, 50, "%.1f");
        cvui::counter(frame, 160, 150, &cy, 50, "%.1f");
        cvui::counter(frame, 160, 200, &fx, 10, "%.1f");
        cvui::counter(frame, 160, 250, &fy, 10, "%.1f");

        cvui::text(frame, 320, 53, "C1");
        cvui::text(frame, 320, 103, "C2");
        cvui::text(frame, 320, 153, "C3");
        cvui::text(frame, 320, 203, "C4");
        cvui::text(frame, 470, 53, "C5");
        cvui::text(frame, 470, 103, "C6");
        cvui::text(frame, 470, 153, "C7");
        cvui::text(frame, 470, 203, "C8");

        cvui::text(frame, 370, 240, "step");
        cvui::counter(frame, 400, 240, &step, 0.001, "%.3f");

        cvui::text(frame, 600, 53, "Frame Scale");
        cvui::counter(frame, 680, 50, &projectionScale, 0.01, "%.2f");

        cvui::counter(frame, 350, 50, &distCoeff1, step, "%.3f");
        cvui::counter(frame, 350, 100, &distCoeff2, step, "%.3f");
        cvui::counter(frame, 350, 150, &distCoeff3, step, "%.3f");
        cvui::counter(frame, 350, 200, &distCoeff4, step, "%.3f");
        cvui::counter(frame, 500, 50, &distCoeff5, step, "%.3f");
        cvui::counter(frame, 500, 100, &distCoeff6, step, "%.3f");
        cvui::counter(frame, 500, 150, &distCoeff7, step, "%.3f");
        cvui::counter(frame, 500, 200, &distCoeff8, step, "%.3f");

        cvui::update();

        imageProcessor->needsUpdate();
        cv::imshow("Distortion Controler", frame);
        int k = cv::waitKey(30);
        if (k == 32 || k == 27 || k == 13) {
            imageProcessor->finish();
            delete imageProcessor;
            cv::destroyAllWindows();
            break;
        }
    }
}

ImageProcessor::ImageProcessor(cv::Mat inputImage, CameraCalibrator *cameraCalibrator) {
    _inputImage = inputImage;
    _cameraCalibrator = cameraCalibrator;
    _needsUpdate = 1;
    _shouldFinish = 0;

    cv::imshow("Corrected Image", _inputImage);
    _processThread = std::thread(&ImageProcessor::processImage, this);
    _processThread.detach();
}

void ImageProcessor::needsUpdate() {
    _needsUpdate = 1;
}

void ImageProcessor::finish() {
    _shouldFinish = 1;
}


ImageProcessor::~ImageProcessor()

{
    cv::imwrite("CorrectedImage.jpg", processedImage);

}

void ImageProcessor::processImage() {
    cv::Mat tmp;
    while (_shouldFinish == 0) {
        if (_needsUpdate != 0) {
            processedImage = _cameraCalibrator->remap2(_inputImage);
            if (_cameraCalibrator->disp_scale_factor > 0) {

                cv::resize(processedImage, tmp, cv::Size(processedImage.size().width / _cameraCalibrator->disp_scale_factor,
                    processedImage.size().height / _cameraCalibrator->disp_scale_factor));
            }

            _needsUpdate = 0;
            cv::imshow("Corrected Image", tmp);
        }
    }
}
