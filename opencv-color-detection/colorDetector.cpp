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
	cv::inRange(hsv,cv::Scalar(35, 50, 30), cv::Scalar(85,255,255),mask);

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
		double area = cv::contourArea(contours[i]);
		if (area < 50)
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
	cv::waitKey(0);
	cv::destroyAllWindows();
	return true;
}

std::string readImage::getName(std::string path)
{
	try
	{
		cv::FileStorage fs;
		bool opened = fs.open(path, cv::FileStorage::READ);
		if (!opened)
		{
			std::cout << "fail to open the file";
			return " ";
		}
		std::string name;
		name = "default";
		cv::FileNode fn = fs["user"]["name"];
		if (!fn.empty() && fn.isString())
		{
			fn >> name;
		}

		return name;
	}
	catch (const cv::Exception& e)
	{
		std::cout << "the file is error" <<e.what()<< std::endl;
		return "error";
	}
}

bool readImage::exportYaml(std::string path)
{
	cv::FileStorage fs;
	bool flag=fs.open(path, cv::FileStorage::WRITE);
	if (!flag)
	{
		return false;
	}
	fs <<"user"<<"{"
		<< "name" << "xwl1"
		<<"password"<<111
		<<"}"
		<< "hsv" << "{"
		<< "lower" << cv::Scalar(30, 50, 0)
		<< "upper" << cv::Scalar(80, 255, 255)
		<< "}"
		<< "kernal" << "{"
		<< "openKernal" << 3
		<< "closeKernal" << 5
		<< "}";

	return true;
}
