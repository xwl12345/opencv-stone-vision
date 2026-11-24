#pragma once
#include <cstdint>
#include <vector>

namespace muyan::depthcam 
{

    struct DepthFrameData 
    {
        int      width = 0;
        int      height = 0;
        float    scaleUnit = 0.0f;    // 原始值 × scaleUnit = 毫米
        uint64_t timestampNs = 0;
        std::vector<uint16_t> depth;    // 原始深度，行优先紧密排列

        bool IsValid() const { return width > 0 && height > 0 && depth.size() == size_t(width) * height; }
    };

} 
