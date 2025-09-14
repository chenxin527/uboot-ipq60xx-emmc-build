本仓库修改自：https://github.com/lgs2007m/uboot-ipq60xx-build

u-boot-2016 源代码基于：https://github.com/gl-inet/uboot-ipq60xx

## 适配设备

此 U-Boot 适配以下 IPQ60xx eMMC 机型：

- 京东云亚瑟（RE-SS-01）

- 京东云雅典娜（RE-CS-02）

- 红米 AX5 JDCloud（RA50）

## 编译步骤

```
sudo apt install build-essential device-tree-compiler python2.7
git clone https://github.com/chenxin527/uboot-ipq60xx-emmc-build.git
cd uboot-ipq60xx-emmc-build
./build.sh
```

PS：mbn 脚本使用 Python2.7 运行，你应该安装并切换到 Python2.7。

编译生成的 U-Boot 文件：uboot-ipq60xx-emmc-build/openwrt-ipq6018-u-boot.mbn

## 网址说明

| 功能        | 网址                          | 备注 |
| :---------- | :---------------------------- | :--- |
| 更新固件    | http://192.168.1.1            |      |
| 更新 ART    | http://192.168.1.1/art.html   |      |
| 更新 CDT    | http://192.168.1.1/cdt.html   |      |
| 更新 IMG    | http://192.168.1.1/img.html   |      |
| 更新 U-Boot | http://192.168.1.1/uboot.html |      |
