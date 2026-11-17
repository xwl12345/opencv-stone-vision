#include "configLoader.h"
#include <iostream>
bool configLoader::loadFromFile(const std::string path, detectorParams& lParams)
{
    cv::FileStorage loadFs;
    bool openFlag=loadFs.open(path, cv::FileStorage::READ);
    if (!openFlag)
    {
        std::cout << "fail to open the file" << std::endl;
    }
    try
    {
        cv::FileNode imagePathNode = loadFs["imagePath"];
        cv::FileNode hsvLowerNode = loadFs["hsvLower"];
        cv::FileNode hsvUpperNode = loadFs["hsvUpper"];
        cv::FileNode openKernelSizeNode = loadFs["openKernelSize"];
        cv::FileNode closeKernelSizeNode = loadFs["closeKernelSize"];
        cv::FileNode minContourAreaNode = loadFs["minContourArea"];

        imagePathNode >> lParams.imagePath;
        hsvLowerNode >> lParams.hsvLower;
        hsvUpperNode >> lParams.hsvUpper;
        openKernelSizeNode >> lParams.openKernelSize;
        closeKernelSizeNode >> lParams.closeKernelSize;
        minContourAreaNode >> lParams.minContourArea;
    }
    catch(cv::Exception e)
    {
        std::cerr << "the operator of file is error" << e.what() << std::endl;
        return false;
    }
    return true;
}

bool configLoader::saveToFile(const std::string path, detectorParams& sParams)
{
    cv::FileStorage saveFs;
    bool openFlag = saveFs.open(path, cv::FileStorage::WRITE);
    if (!openFlag)
    {
        std::cout << "fail to open the file" << std::endl;
    }
    try
    {
        saveFs << "{"
            << "imagePath" << sParams.imagePath
            << "hsvLower" << sParams.hsvLower
            << "hsvUpper" << sParams.hsvUpper
            << "openKernelSize" << sParams.openKernelSize
            << "closeKernelSize" << sParams.closeKernelSize
            << "minContourArea" << sParams.minContourArea
            << "}";
    }
    catch (cv::Exception e)
    {
        std::cerr << "the operator of file is error" << e.what() << std::endl;
        return false;
    }
    return true;
}
