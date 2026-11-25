#include "SdkSession.h"
#include <vcamera/cameraUtils.h>
#include <mutex>

namespace 
{
    std::mutex  s_mtx;        // 保护下面两个变量
    int         s_ref = 0;    // 引用计数：全进程这一份
    std::string s_lastErr;    // 最近一次 Acquire 失败信息（UTF-8）
}
namespace muyan::depthcam 
{

    bool SdkSession::Acquire(bool useDirectCall) 
    {
        std::lock_guard<std::mutex> lock(s_mtx);
        if (s_ref > 0)
        {
            ++s_ref;
            return true;
        }
        vcamera::CameraApiStatus st = vcamera::CameraUtils::Init(useDirectCall);
        if (st.IsSuccess())
        {
            s_ref = 1;
            return true;
        }
        else
        {
            s_lastErr = st.message();
            return false;
        }
    }

    void SdkSession::Release() 
    {
        std::lock_guard<std::mutex> lock(s_mtx);
        if (s_ref == 0)
        {
            return;
        }
        s_ref -= 1;
        if(s_ref==0)
        {
            vcamera::CameraUtils::Uninit();
            s_lastErr.clear();
        }
    }

    bool SdkSession::IsHeld() {
        std::lock_guard<std::mutex> lock(s_mtx);
        if (s_ref > 0)
        {
            return true;
        }
        return false;            
    }

    std::string SdkSession::LastError() 
    {
        std::lock_guard<std::mutex> lock(s_mtx);
        return s_lastErr;               
    }

}