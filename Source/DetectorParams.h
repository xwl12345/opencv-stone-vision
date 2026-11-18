#pragma once
#include <opencv2/opencv.hpp>
struct DetectorParams
{
	std::string imagePath="D:\\VS_Project\\opencv-stone-vision\\images\\blue_block.jpg";
	cv::Scalar hsvLower{60,5,5};
	cv::Scalar hsvUpper{150,255,255};
	int openKernelSize = 3;
	int closeKernelSize = 5;
	double minContourArea = 50.00;
};