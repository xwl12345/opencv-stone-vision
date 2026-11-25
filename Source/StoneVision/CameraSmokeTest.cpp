#include "CameraSmokeTest.h"
#include "DepthCameraControl.h"
#include <iostream>

bool RunCameraSmokeTest()
{
	if (!DepthCameraControl::InitSdk())
	{
		std::cout << "InitSdk 失败: " << DepthCameraControl::SdkLastError() << std::endl;
		return false;
	}

	auto devs = DepthCameraControl::DiscoverCameras();
	std::cout << "发现相机: " << devs.size() << std::endl;

	bool ok = true;
	{   // ctrl 必须在 ShutDownSdk 前析构（Uninit 前所有 Camera 已析构）
		DepthCameraControl ctrl;
		std::cout << "OpenBySerial: " << ctrl.OpenBySerial("0") << std::endl;
		std::cout << "Connect: " << ctrl.Connect()
			<< " | " << ctrl.LastError() << std::endl;

		ctrl.StartCapture();
		std::cout << ctrl.LastError() << std::endl;
		ctrl.StopCapture();
		std::cout << ctrl.LastError() << std::endl;
		if (!ctrl.IsCapturing())
		{
			std::cout << "no capturing" << std::endl;
		}

		std::cout << "State=" << int(ctrl.State())
			<< " IsConnected=" << ctrl.IsConnected() << std::endl;

		std::cout << "Disconnect: " << ctrl.Disconnect() << std::endl;

		DepthFrameData frame;
		ctrl.GetLastDepth(frame);
		std::cout << ctrl.LastError() << std::endl;

		ctrl.SetFrameCallback([](const DepthFrameData&) { std::cout << "cb" << std::endl; });
		ctrl.StopCapture();
		ctrl.GetLastDepth(frame);
		std::cout << ctrl.LastError() << std::endl;
	}

	DepthCameraControl::ShutDownSdk();
	return ok;
}
