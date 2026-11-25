#pragma once
#ifdef DEPTH_CAMERA_EXPORT
#define DEPTH_CAMERA_API __declspec(dllexport)
#else
#define DEPTH_CAMERA_API __declspec(dllimport)
#endif
