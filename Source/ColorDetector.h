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
private:
	DetectorParams m_params;
};