#include "ColorDetector.h"

void onMouse(int event, int x, int y,int flag,void* userdata)
{
	if (event != cv::EVENT_LBUTTONDOWN)
	{
		return;
	}

	cv::Mat* hsvPtr = static_cast<cv::Mat*> (userdata);
	{
		if (hsvPtr == nullptr)
		{
			return;
		}
		cv::Vec3b hsvValue=hsvPtr->at<cv::Vec3b>(y, x);
		for (int i = 0;i < 3;++i)
		{
			std::cout << (int)hsvValue[i] << std::endl;
		}
	}

}
ColorDetector::ColorDetector(const DetectorParams& params):m_params(params)
{
}

bool ColorDetector::getMat()
{
	//读取图片
	cv::Mat target = cv::imread(m_params.imagePath);
	if (target.empty())
	{
		std::cout << "读取失败" << std::endl;
		return false;
	}
	//BGR转化为HSV
	cv::Mat hsv;
	cv::cvtColor(target, hsv, cv::COLOR_BGR2HSV);
	//获取掩膜
	cv::Mat mask;
	cv::inRange(hsv, m_params.hsvLower, m_params.hsvUpper,mask);

	std::cout << "before: " << cv::countNonZero(mask)<<std::endl;
	//创建3*3椭圆核来做开运算去噪点
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(m_params.openKernelSize,m_params.openKernelSize));
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

	cv::Mat closeKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(m_params.closeKernelSize,m_params.closeKernelSize));
	cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, closeKernel);
	//获取轮廓
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	if (contours.empty())
	{
		std::cout << "fail to get contours" << std::endl; 
		return false;
	}
	std::cout << "after:" << cv::countNonZero(mask)<<std::endl;
	cv::Mat result = target.clone();
	int count = 0;
	for(int i=0;i<contours.size();++i)
	{
		double area = cv::contourArea(contours[i]);
		if (area < m_params.minContourArea)
		{
			continue;
		}
		cv::drawContours(result, contours, i, cv::Scalar(0, 0, 255));
		cv::Rect boundRec = boundingRect(contours[i]);
		cv::rectangle(result, boundRec, cv::Scalar(70, 70, 70));
		cv::putText(result, std::to_string(count), cv::Point(boundRec.x, boundRec.y), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255));
			++count;
	}
	std::cout << "total:" << count << std::endl;
	std::cout << contours.size() << std::endl;
	cv::imshow("contours", result);
	cv::imshow("origin", target);
	cv::imshow("mask", mask);
	cv::setMouseCallback("origin", onMouse, &target);

	cv::waitKey(0);
	cv::destroyAllWindows();
	return true;
}
