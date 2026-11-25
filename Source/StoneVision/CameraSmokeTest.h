#pragma once

// 相机封装的冒烟测试：走一遍 初始化 → 发现 → 打开 → 连接 → 采集 → 取帧 → 断开 → 反初始化
// 返回值：true 表示全程没有走到失败分支（不代表真机连接成功，无相机时会打印 Not exist）
bool RunCameraSmokeTest();
