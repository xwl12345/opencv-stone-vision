#include "DepthCameraControl.h"
#include "vcamera/version.h"
#include "vcamera/cameraUtils.h"
#include "vcamera/camera.h"
#include "SdkSession.h"
#include <iostream>
#include <mutex>

namespace
{
	std::mutex    s_sdkErrMtx;
	std::string   s_sdkLastError;   // 进程级静态操作的错误
}

using namespace muyan::depthcam;
//int main() 
//{
//    if (!SdkSession::Acquire(true))
//    {
//        std::cout << SdkSession::LastError() << std::endl;
//        return 1;
//    }
//    std::cout << "SDK 版本: " << vcamera::GetVersionString() << std::endl;
//    SdkSession::Acquire();
//    std::cout<<SdkSession::IsHeld()<<std::endl;
//    SdkSession::Release();
//    std::cout << SdkSession::IsHeld() << std::endl;
//
//    auto devices = vcamera::CameraUtils::DiscoverCameras();
//    std::cout << "发现相机数量: " << devices.size() << std::endl;
//   SdkSession::Release();
//   std::cout << SdkSession::IsHeld() << std::endl;
//   SdkSession::Acquire();
//   std::cout << SdkSession::IsHeld() << std::endl;
//
//    return 0;
//}
struct DepthCameraControl::Impl
{
	vcamera::Camera camera; 

	bool opened = false;//是否拿到设备句柄(open成功不代表连接成功)
	std::string serial, ip;
	std::string model;//Connect成功后从GetCameraInfo缓存

	DepthCameraState state = DepthCameraState::Unknown;
	std::string lastError;

	mutable std::mutex mtx;

	void SetState(DepthCameraState s);
	void SetError(const std::string& msg);
	void RegisterEventCallback();
};

muyan::depthcam::DepthCameraControl::DepthCameraControl()
{
	impl = std::make_unique<Impl>();
}

muyan::depthcam::DepthCameraControl::~DepthCameraControl() = default;

bool muyan::depthcam::DepthCameraControl::InitSdk(bool useDirectCall)
{
	bool initFlag =SdkSession::Acquire(useDirectCall);
	if (!initFlag)
	{
		std::lock_guard<std::mutex> lock(s_sdkErrMtx);
		s_sdkLastError = SdkSession::LastError();
		return false;
	}
	return true;
}

void muyan::depthcam::DepthCameraControl::ShutDownSdk()
{
	SdkSession::Release();
	return;
}

std::vector<DepthCameraDesc> muyan::depthcam::DepthCameraControl::DiscoverCameras()
{
	if (!SdkSession::IsHeld())
	{
		std::lock_guard<std::mutex> lock(s_sdkErrMtx);
		s_sdkLastError=SdkSession::LastError();
		return {};
	}
	std::vector<vcamera::CameraInfo> infoVec = vcamera::CameraUtils::DiscoverCameras();

	std::vector<DepthCameraDesc> DiscoveredVec;
	DiscoveredVec.resize(infoVec.size());
	for (int i=0;i<infoVec.size();++i)
	{
		DiscoveredVec[i].ip = infoVec[i].network_info.ip;
		DiscoveredVec[i].serialNumber = infoVec[i].serial_number;
		DiscoveredVec[i].model = infoVec[i].model;
		DiscoveredVec[i].firmwareVersion = infoVec[i].firmware_version;

	}
	return DiscoveredVec;
}

std::string muyan::depthcam::DepthCameraControl::SdkLastError()
{
	std::lock_guard<std::mutex> lock(s_sdkErrMtx);
	return s_sdkLastError;
}

bool muyan::depthcam::DepthCameraControl::OpenBySerial(const std::string serialNumber)
{
	if (!SdkSession::IsHeld())//后面可以看看序列号格式改判断条件
	{
		impl->SetError("请先InitSDK");

		return false;
	}
	impl->camera = vcamera::CameraFactory::GetCameraBySerialNumber(serialNumber);
	impl->opened = true;
	impl->SetState(DepthCameraState::Closed);
	impl->RegisterEventCallback();

	return true;
}

bool muyan::depthcam::DepthCameraControl::OpenByIp(const std::string ipAddress)
{
	if (!SdkSession::IsHeld())
	{
		impl->SetError("请先InitSDK");
		return false;
	}
	impl->camera = vcamera::CameraFactory::GetCameraByIpAddress(ipAddress);
	impl->opened = true;
	impl->SetState(DepthCameraState::Closed);
	impl->RegisterEventCallback();
	return true;
}

bool muyan::depthcam::DepthCameraControl::Connect()
{
	if (!SdkSession::IsHeld())
	{
		impl->SetError("SDK尚未初始化");
		return false;
	}
	if (!impl->opened)
	{
		impl->SetError("尚未opened,请先OpenBySerial/OpenByIp");
		return false;
	}
	if (IsConnected())
	{
		impl->SetError("已连接，请勿重复连接");
		return false;
	}
	auto st = impl->camera.Connect();
	if (!st.IsSuccess())
	{
		if (IsConnected())
		{
			impl->SetState(DepthCameraState::Occupied);
			impl->SetError(st.message());
			return false;
		}
		if (st.code() == vcamera::CameraApiStatusCode::NotExist)
		{
			impl->SetState(DepthCameraState::NotFound);
			impl->SetError(st.message());
			return false;
		}
		if (st.code() == vcamera::CameraApiStatusCode::NullCameraHandle || st.code() == vcamera::CameraApiStatusCode::CameraInvalid)
		{
			impl->SetState(DepthCameraState::Error);
			impl->SetError(st.message());
		}
		impl->SetState(DepthCameraState::Error);
		impl->SetError(st.message());
		return false;
	}
	else
	{
		vcamera::CameraInfo info;
		impl->camera.GetCameraInfo(info);
		impl->model = info.model;
		impl->SetState(DepthCameraState::Opened);
		return true;

	}
}

bool muyan::depthcam::DepthCameraControl::Disconnect()
{
	if (!IsConnected())
	{
		impl->SetError("未连接，无法断开连接");
		return false;
	}
	auto ret=impl->camera.Disconnect();
	if (ret.IsSuccess())
	{
		impl->SetState(DepthCameraState::Closed);
		impl->opened = false;
		return true;
	}
	return false;
}

bool muyan::depthcam::DepthCameraControl::IsConnected() const
{
	if (impl->state == DepthCameraState::Opened || impl->state == DepthCameraState::Capturing)
	{
		return true;
	}
	return false;
}

DepthCameraState muyan::depthcam::DepthCameraControl::State() const
{
	return impl->state;
}

std::string muyan::depthcam::DepthCameraControl::DeviceModel() const
{
	std::lock_guard<std::mutex> lock(impl->mtx);
	return impl->model;
}

bool muyan::depthcam::DepthCameraControl::EnableAutoReconnect()
{
	if (impl->opened && !IsConnected())
	{
		auto st=impl->camera.EnableReconnectWhenOffline();
		if(!st.IsSuccess())
		{
			return false;
		}
		return true;
	}
	return false;
}

bool muyan::depthcam::DepthCameraControl::EnableAutoReconnect(int maxAttempts)
{
	if (impl->opened && !IsConnected())
	{
		if(maxAttempts>0)
		{
			uint32_t maxTime = maxAttempts;
			auto st=impl->camera.EnableReconnectWhenOfflineWithMaxAttempts(maxTime);
			if (!st.IsSuccess())
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

bool muyan::depthcam::DepthCameraControl::EnableAutoReconnect(double maxSeconds)
{

	if (impl->opened && !IsConnected())
	{
		if (maxSeconds > 0)
		{
			auto st=impl->camera.EnableReconnectWhenOfflineWithTimeoutSeconds(maxSeconds);
			if (!st.IsSuccess())
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

bool muyan::depthcam::DepthCameraControl::DisableAutoReconnect()
{
	if (impl->opened && !IsConnected())
	{
		auto ret=impl->camera.DisableReconnectWhenOffline();
		if (ret.IsSuccess())
		{
			return true;
		}
		else
		{
			impl->SetError(ret.message());
			return false;
		}
	}
}

bool muyan::depthcam::DepthCameraControl::SetRecoverCaptureAfterReconnect(bool enable)
{
	if (impl->opened && !IsConnected())
	{
		auto st=impl->camera.SetReconnectCaptureRecoveryEnabled(enable);
		if (!st.IsSuccess())
		{
			return false;
		}
		return true;
	}
	return false;
}

bool muyan::depthcam::DepthCameraControl::StartCapture()
{
	return false;
}

bool muyan::depthcam::DepthCameraControl::StopCapture()
{
	return false;
}

bool muyan::depthcam::DepthCameraControl::IsCapturing() const
{
	return false;
}

bool muyan::depthcam::DepthCameraControl::FireSoftwareTrigger()
{
	return false;
}

bool muyan::depthcam::DepthCameraControl::GetLastDepth(DepthFrameData& out) const
{
	return false;
}

void muyan::depthcam::DepthCameraControl::SetFrameCallback(std::function<void(const DepthFrameData&)> cb)
{
}

std::string muyan::depthcam::DepthCameraControl::LastError() const
{

	return impl->lastError;
}

void DepthCameraControl::Impl::SetState(DepthCameraState s)
{
	std::lock_guard<std::mutex> lock(mtx);
	this->state = s;
}

void DepthCameraControl::Impl::SetError(const std::string& msg)
{
	this->lastError = msg;
}

void DepthCameraControl::Impl::RegisterEventCallback()
{
	Impl* self = this;
	self->camera.RegisterCameraEventCallback([self](vcamera::CameraEventCode code, int err) {
		switch (code)
		{
		case vcamera::CameraEventCode::Closed:
			self->SetState(DepthCameraState::Closed);
			break;
		case vcamera::CameraEventCode::Opened:
			self->SetState(DepthCameraState::Opened);
			break;
			/*	case vcamera::CameraEventCode::Started:
					self->SetState(DepthCameraState::);*/
			break;
			/*case vcamera::CameraEventCode::Stopped:
				break;*/
		case vcamera::CameraEventCode::Offlined:
			self->SetState(DepthCameraState::Offlined);
			break;
		case vcamera::CameraEventCode::Error:
			self->SetState(DepthCameraState::Error);
			break;
		case vcamera::CameraEventCode::Reconnecting:
			self->SetState(DepthCameraState::Reconnecting);
			break;
		case vcamera::CameraEventCode::ReconnectSuccess:
			self->SetState(DepthCameraState::ReconnectSuccess);
			break;
		default:
			break;
		}
		});
	
}
