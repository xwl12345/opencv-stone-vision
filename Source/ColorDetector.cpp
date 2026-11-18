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
	self->update();
}
ColorDetector::ColorDetector(const DetectorParams& params) :m_params(params)
, m_target(cv::imread(m_params.imagePath))
, m_openKernel(cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(m_params.openKernelSize, m_params.openKernelSize)))
, m_closeKernel(cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(m_params.closeKernelSize, m_params.closeKernelSize)))
, m_lowerH(static_cast<int>(m_params.hsvLower[0]))
, m_upperH(static_cast<int>(m_params.hsvUpper[0]))
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
	update();
	cv::createTrackbar("lowerH trackbar", "mask",&m_lowerH, 179, OnTrackBar,this);
	cv::createTrackbar("upperH trackbar", "mask", &m_upperH, 179, OnTrackBar, this);

	cv::setMouseCallback("origin", onMouse, &m_target);

	cv::waitKey(0);
	cv::destroyAllWindows();
	return true;
}

bool ColorDetector::update()
{
	cv::Mat mask;
	cv::inRange(m_hsv, cv::Scalar(m_lowerH, 5, 5), cv::Scalar(m_upperH,255,255),mask);

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
		cv::drawContours(result, contours, i, cv::Scalar(0, 0, 255));
		cv::Rect boundRec = boundingRect(contours[i]);
		cv::rectangle(result, boundRec, cv::Scalar(70, 70, 70));
		cv::putText(result, std::to_string(count), cv::Point(boundRec.x, boundRec.y), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255));
		++count;
	}
	cv::imshow("contour", result);
	return true;
}
