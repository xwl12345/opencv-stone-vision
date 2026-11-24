#include "DepthCameraControl.h"
#include <iostream>
using namespace muyan::depthcam;
int main()
{
    if (!DepthCameraControl::InitSdk())
    {
        return 1;
    }
    auto devs = DepthCameraControl::DiscoverCameras();
    std::cout << "发现相机: " << devs.size() << std::endl;
    {   // ctrl 必须在 ShutdownSdk 前析构（Uninit 前所有 Camera 已析构）
        DepthCameraControl ctrl;
        std::cout << "OpenBySerial: " << ctrl.OpenBySerial("0") << std::endl;
        std::cout << "Connect: " << ctrl.Connect()
            << " | " << ctrl.LastError() << std::endl;
        bool captureFlag = ctrl.StartCapture();
        std::cout << ctrl.LastError() << std::endl;
        ctrl.StopCapture();
        std::cout << ctrl.LastError() << std::endl;
        if (!ctrl.IsCapturing())
        {
            std::cout << "no capturing" << std::endl;
        }
        std::cout << "State=" << int(ctrl.State())
            << " IsConnected=" << ctrl.IsConnected() << std::endl;
        std::cout << "Disconnect: " << ctrl.Disconnect() << std::endl;   // 未连接时行为未知，观察
        std::cout << "StartCapture桩: " << ctrl.StartCapture() << std::endl;  // 期望 0 + 桩提示
    }
    DepthCameraControl::ShutDownSdk();
    return 0;
}