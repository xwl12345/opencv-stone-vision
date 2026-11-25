#pragma once
#include <string>
#include "DepthCamera/DepthCameraExport.h"
class DEPTH_CAMERA_API SdkSession
{
public:
	SdkSession() = delete;

    // 引用 +1；首次调用真正执行 SDK Init。失败返回 false（此时计数不变）
    static bool Acquire(bool useDirectCall = true);

    // 引用 -1；减到 0 时真正执行 SDK Uninit
    static void Release();

    // 当前 SDK 是否处于已初始化状态（计数 > 0）
    static bool IsHeld();

    // 最近一次 Acquire 的失败信息，UTF-8；没有错误则返回空串
    static std::string LastError();

};
