#include "colorDetector.h"
#include "configLoader.h"
#include "detectorParams.h"
int main()
{
	const std::string path = "D:\\VS_Project\\opencv-color-detection\\option.yaml";
	//const std::string path = "D:\\VS_Project\\opencv-color-detection\\images\\blue_block.jpg";
	colorDetector detector;
	configLoader loader;
	detectorParams parms;
	bool flag= detector.exportYaml(path);
	std::cout << flag<<std::endl;
	std::string name= detector.getName(path);
	std::cout << name;
	return 0;
}