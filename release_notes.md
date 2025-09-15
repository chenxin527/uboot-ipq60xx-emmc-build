## 更新日志

### 新特性

- feat: 将 U-Boot WebUI 翻译为简体中文
- feat: 优化 U-Boot WebUI，添加侧边导航栏，切换为深色背景
- feat: 支持刷写更大的 U-Boot 文件（不超过 1024KB）
- feat: 支持刷写内核大小为 12MB 的固件

## 支持设备

- 京东云亚瑟（RE-SS-01）
- 京东云雅典娜（RE-CS-02）
- 红米 AX5 JDCloud（RA50）

## 文件说明

两个 U-Boot 功能一致，只是 WebUI 不同。

full 版本的 WebUI 有侧边导航栏，深色背景，界面更美观，但文件大小超过 640KB，需扩容 0:APPSBL 分区后刷入。

tiny 版本的 WebUI 无侧边导航栏，白色背景，界面较简陋，但文件大小小于 640KB，无需扩容 0:APPSBL 分区，可直接刷入。
