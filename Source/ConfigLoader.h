#pragma once
#include "DetectorParams.h"
#include <string>
class ConfigLoader
{
public:
	bool loadFromFile(const std::string& path, DetectorParams& lParams);
	bool saveToFile(const std::string& path,const DetectorParams& sParams);
};