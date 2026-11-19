#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "DetectorParams.h"
class ColorDetector
{
public:
	explicit ColorDetector(const DetectorParams& params);
	bool getMat();
	bool update();
	 bool isReady() const;
private:
	DetectorParams m_params;
	cv::Mat m_target;
	cv::Mat m_hsv;
	cv::Mat m_openKernel;
	cv::Mat m_closeKernel;
	bool m_ready;
};