#pragma once
#include "detectorParams.h"
#include <string>
class configLoader
{
	bool loadFromFile(const std::string path, detectorParams& lParams);
	bool saveToFile(const std::string path, detectorParams& sParams);
};