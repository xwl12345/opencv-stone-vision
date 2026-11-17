#pragma once
#include <opencv2/opencv.hpp>
struct detectorParams
{
	std::string imagePath;
	cv::Scalar hsvLower{30,50,255};
	cv::Scalar hsvUpper{80,255,255};
	int openKernelSize = 3;
	int closeKernelSize = 5;
	double minContourArea = 50.00;
};