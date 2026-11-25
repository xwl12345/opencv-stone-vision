#include "ColorDetector.h"
#include "ConfigLoader.h"
#include "DetectorParams.h"
#include "CameraSmokeTest.h"
#include <opencv2/core/utils/logger.hpp>
#include <iostream>

int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

	std::cout << "相机封装自测 " << std::endl;
	bool cameraOk = RunCameraSmokeTest();
	std::cout << "自测结果: " << (cameraOk ? "通过" : "失败") << std::endl;

	const std::string yamlPath = "D:\\VS_Project\\opencv-stone-vision\\option.yaml";
	//const std::string path = "D:\\VS_Project\\opencv-stone-vision\\images\\blue_block.jpg";
	ConfigLoader loader;
	DetectorParams parms;
	//bool flag = loader.saveToFile(yamlPath, parms);
	bool flag = loader.loadFromFile(yamlPath,parms);
	std::cout << flag<<std::endl;
	ColorDetector detector(parms);
	detector.getMat();
	return 0;
}
