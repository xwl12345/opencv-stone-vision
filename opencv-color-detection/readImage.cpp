#include "readImage.h"

bool readImage::getMat(std::string path)
{
	//读取图片
	cv::Mat target = cv::imread(path);
	if (target.empty())
	{
		std::cout << "读取失败" << std::endl;
		return false;
	}
	std::cout << target.rows <<std::endl;
	std::cout << target.cols << std::endl;
	//BGR转化为HSV
	cv::Mat hsv;
	cv::cvtColor(target, hsv, cv::COLOR_BGR2HSV);
	//获取掩膜
	cv::Mat mask;
	cv::inRange(hsv,cv::Scalar(100, 50, 30), cv::Scalar(130,255,255),mask);

	std::cout << "before: " << cv::countNonZero(mask)<<std::endl;
	//创建3*3椭圆核来做开运算去噪点
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

	cv::Mat closeKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
	cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, closeKernel);
	//获取轮廓
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	if (contours.empty())
	{
		//std::cout << "未得到轮廓" << std::endl; 
		return false;
	}
	std::cout << "after:" << cv::countNonZero(mask)<<std::endl;
	cv::Mat result = target.clone();
	int count = 0;
	for(int i=0;i<contours.size();++i)
	{
		double area = cv::contourArea(contours[0]);
		if (area < 50)
		{
			continue;
		}
		cv::drawContours(result, contours, i, cv::Scalar(0, 0, 255));
		cv::Rect boundRec = boundingRect(contours[i]);
		cv::rectangle(result, boundRec, cv::Scalar(70, 70, 70));
		cv::putText(result, std::to_string(100), cv::Point(boundRec.x, boundRec.y), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0));\
			++count;
	}
	std::cout << "total:" << count << std::endl;
	std::cout << contours.size() << std::endl;
	cv::imshow("contours", result);
	cv::imshow("origin", target);
	cv::imshow("mask", mask);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return true;
}
