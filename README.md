# opencv-stone-vision

石材工件的视觉识别 demo：读图 → HSV 阈值分割 → 形态学处理 → 轮廓提取 → 多边形逼近与最小外接矩形 → 像素尺寸标定，

另含一个接入图漾（Percipio）Vcamera 深度相机 SDK 的 `DepthCamera` 工程，用于验证 SDK 的可编译、可运行。

## 环境要求



* Windows 10/11

* Visual Studio 2026（工程为 `.slnx`，`VCProjectVersion 18.0`）

* 工具集：当前两个项目统一为 `v143`（VS2022 工具集，14.44）

* 平台：仅 x64（第三方库只提供 64 位版本，Win32 配置已移除）

* C++ 标准：C++20

## 目录结构



```
opencv-stone-vision.slnx
option.yaml             检测参数与测试图片路径
images/                 测试图片
Source/
  StoneVision/          StoneVision：主程序、颜色检测、配置加载
  DepthCamera/          相机 SDK 工程（封装 DepthCameraControl，test.cpp 为入口）
ThirdParty/             第三方库（不入库，需自行放置，见下节）
Int/ Output/            构建中间目录与输出（不入库）
```

## 第三方库准备

仓库不含第三方库，clone 后需要把下面两个目录放到 `ThirdParty\` 下，保持这个相对结构：



```
ThirdParty\
├─ OpenCV\
│  ├─ include\           opencv2 头文件
│  ├─ lib\               opencv_world455.lib / opencv_world455d.lib
│  └─ dll\               opencv_world455.dll / opencv_world455d.dll 等
└─ VcameraSDK-26.1.15\
   └─ cpp\
      ├─ include\vcamera\         SDK 头文件
      └─ win\
         ├─ Debug\{lib,bin}\      ucvcameraapi.lib + 运行库
         └─ Release\{lib,bin}\
```

获取途径：OpenCV 4.5.5 的 Windows 预编译包、图漾 Vcamera SDK（版本 26.1.15），按团队约定从内部分享盘 / 网盘获取，或参考官方文档

[https://doc.percipio.xyz/cam/latest/vcamera/vc\_userguide.html](https://doc.percipio.xyz/cam/latest/vcamera/vc_userguide.html)。

工程里引用的是 `$(SolutionDir)ThirdParty\...` 宏路径，所以仓库放在哪个盘、哪个目录都能编译，只要保持上面的相对结构不变。

## 运行前：拷贝运行时文件

dll 不在输出目录里，需要从 `ThirdParty` 拷到 exe 所在目录（`Output\x64\<配置>`）：



| 来源                                                    | 目标                   | 内容                                                                                                                                             |
| ----------------------------------------------------- | -------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| `ThirdParty\VcameraSDK-26.1.15\cpp\win\Debug\bin\*`   | `Output\x64\Debug`   | 7 个 dll（`ucvcameraapi`、`ucvbase`、`tycam`、`tyimgproc`、`ucvdatamodel`、`ucvcameraclientdirect`、`VisionTK`）+ `feature_config.xml`、`FeatureMap.xml` |
| `ThirdParty\VcameraSDK-26.1.15\cpp\win\Release\bin\*` | `Output\x64\Release` | 同上（Release 版本）                                                                                                                                 |
| `ThirdParty\OpenCV\dll\opencv_world455d.dll`          | `Output\x64\Debug`   | OpenCV Debug 运行库                                                                                                                               |
| `ThirdParty\OpenCV\dll\opencv_world455.dll`           | `Output\x64\Release` | OpenCV Release 运行库                                                                                                                             |

目前这一步是手工拷贝，后续可以改成生成后事件自动完成。

## 检测参数

`option.yaml` 保存检测参数：`imagePath`（测试图片路径）、`hsvLower` / `hsvUpper`（HSV 阈值）、`openKernelSize` / `closeKernelSize`（开闭运算核大小）、`minContourArea`（最小轮廓面积）。

`option.yaml` 里的 `imagePath` 和 `Source/StoneVision/main.cpp` 里的 yaml 路径目前都是绝对路径，换机器或换目录需要改成新路径。

