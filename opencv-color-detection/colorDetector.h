#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
class colorDetector
{
public:
	bool getMat(std::string path);
	std::string getName(std::string path);
	bool exportYaml(std::string path);
};