#include "ColorDetector.h"
#include "ConfigLoader.h"
#include "DetectorParams.h"
#include <opencv2/core/utils/logger.hpp>

int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
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