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
void  OnTrackBar(int pos, void* userdata)
{
	
	ColorDetector* self = static_cast<ColorDetector*>(userdata);
	if (self == nullptr)
	{
		return;
	}
	if (self->isReady() == false)
	{
		return;
	}
	self->update();
}
ColorDetector::ColorDetector(const DetectorParams& params) :m_params(params)
, m_target(cv::imread(m_params.imagePath))
, m_openKernel(cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(m_params.openKernelSize, m_params.openKernelSize)))
, m_closeKernel(cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(m_params.closeKernelSize, m_params.closeKernelSize)))
,m_ready(false)
{

}

bool ColorDetector::getMat()
{
	//读取图片
	if (m_target.empty())
	{
		std::cout << "读取失败" << std::endl;
		return false;
	}
	//BGR转化为HSV
	cv::cvtColor(m_target, m_hsv, cv::COLOR_BGR2HSV);

	cv::imshow("origin", m_target);

	cv::namedWindow("mask");
	cv::namedWindow("contour");
	cv::createTrackbar("lowerH", "mask",0, 179, OnTrackBar,this);
	cv::setTrackbarPos("lowerH", "mask", static_cast<int>(m_params.hsvLower[0]));
	cv::createTrackbar("upperH", "mask", 0, 179, OnTrackBar, this);
	cv::setTrackbarPos("upperH", "mask", static_cast<int>(m_params.hsvUpper[0]));
	update();
	m_ready = true;
	cv::setMouseCallback("origin", onMouse, &m_target);

	cv::waitKey(0);
	cv::destroyAllWindows();
	return true;
}

bool ColorDetector::update()
{	
	cv::Mat mask;
	int lowerH = cv::getTrackbarPos("lowerH","mask");
	int upperH = cv::getTrackbarPos("upperH", "mask");
	int64_t t1 = cv::getTickCount();
	cv::inRange(m_hsv, cv::Scalar(lowerH, 5, 5), cv::Scalar(upperH,255,255),mask);
	int64_t t2 = cv::getTickCount();
	//std::cout << (t2 - t1) * 1000 / cv::getTickFrequency() << std::endl;
	//3*3椭圆核来做开运算去噪点
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, m_openKernel);
	//5*5
	cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, m_closeKernel);
	//获取轮廓椭圆核来做闭运算
	std::vector<std::vector<cv::Point>> contours;

	cv::imshow("mask", mask);


	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	if (contours.empty())
	{
		std::cout << "fail to get contours" << std::endl;
		return false;
	}
	cv::Mat result = m_target.clone();
	int count = 0;
	for (int i = 0;i < contours.size();++i)
	{
		double area = cv::contourArea(contours[i]);
		if (area < m_params.minContourArea)
		{
			continue;
		}
		double length = cv::arcLength(contours[i], true);
		double epsilon = length * 0.01;
		std::vector<cv::Point> approxContour;
		cv::approxPolyDP(contours[i], approxContour, epsilon, true);
		std::cout << approxContour.size() << std::endl;

		cv::drawContours(result, contours,i, cv::Scalar(0, 0, 255));
		cv::polylines(result, approxContour, true, cv::Scalar(100, 0, 200), 2);

		cv::RotatedRect rotRect=cv::minAreaRect(contours[i]);
		m_params.pixelScale = m_params.referenceMm/rotRect.size.width  ;
		double realWidth = rotRect.size.width * m_params.pixelScale;
		double realHeight = rotRect.size.height * m_params.pixelScale;
		std::cout << "real width:" << realWidth << std::endl;
		std::cout << "real height:" << realHeight << std::endl;
		std::cout << "angle: " << rotRect.angle << std::endl;
		std::cout << "center: " << rotRect.center << std::endl;
		std::cout << "size: " << rotRect.size << std::endl;
		cv::Mat boxMat;
		cv::boxPoints(rotRect, boxMat);
		cv::Mat boxInt;
		boxMat.convertTo(boxInt, CV_32S);
		cv::polylines(result, boxInt, true, cv::Scalar(0, 255, 0), 2);
		cv::circle(result, rotRect.center, 1, cv::Scalar(0, 0, 255));
		cv::Mat rotateImg = result.clone();
		rotate(result, rotateImg, rotRect.center, rotRect.angle, 1.0);
		cv::imshow("rotate", rotateImg);

		cv::Rect boundRec = boundingRect(contours[i]);
		cv::rectangle(result, boundRec, cv::Scalar(70, 70, 70));
		cv::putText(result, std::to_string(count), cv::Point(boundRec.x, boundRec.y), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255));
		++count;
	}
	cv::imshow("contour", result);

	

	return true;
}

bool ColorDetector::rotate(cv::InputArray inputSrc,cv::Mat& outPut, cv::Point2f center, double angle, double scale)
{
	
	cv::Mat rotMat=cv::getRotationMatrix2D(center, angle, scale);
	cv::warpAffine(inputSrc, outPut, rotMat, m_target.size());
	return true;
}

 bool ColorDetector::isReady() const
{
	return m_ready;
}
