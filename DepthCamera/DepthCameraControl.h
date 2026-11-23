#pragma once
#include "DepthFrame.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
enum class DepthCameraState
{
	Unknow,
	NotFound,
	Occupied,
	Opened,
	Closed,
	Capturing,
	offlined,
	Error
};
struct DepthCameraDesc
{
	std::string serialNumber, model, ip, firmwareVersion;
};

class DepthCameraControl
{
public:
	DepthCameraControl();
	~DepthCameraControl();
	DepthCameraControl(const DepthCameraControl& other) = delete;
	DepthCameraControl& operator=(const DepthCameraControl&) = delete;

	static bool InitSdk(bool useDirectCall = true);
	static void ShutDownSdk();
	static std::vector<DepthCameraControl> DiscoverCameras();
	static std::string SdkLastError();

	//打开/连接
	bool OpenBySerial(const std::string serialNumber);//序列号连接
	bool OpenByIp(const std::string ipAddress);
	bool Connect();
	bool Disconnect();
	bool IsConnected() const;
	DepthCameraState State() const;
	std::string DeviceModel() const;

	//断线重连：必须在connect前调用
	bool EnableAutoReconnect(); //无限次
	bool EnableAutoReconnect(int maxAttempts);//按次数
	bool EnableAutoReconnect(double maxSeconds);//按时长
	bool DisableAutoReconnect();
	bool SetRecoverCaptureAfterReconnect(bool enable);

	//采集
	bool StartCapture();
	bool StopCapture();
	bool IsCaoturing() const;
	bool FireSoftwareTrigger();
	bool GetLastDepth(DepthFrameData& out) const;
	void SetFrameCallback(std::function<void(const DepthFrameData&)> cb);

	const std::string& LastError() const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};
