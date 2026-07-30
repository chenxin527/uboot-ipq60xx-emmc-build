> [!NOTE]
>
> 此项目已不再更新，请移步新项目：https://github.com/chenxin527/uboot-qsdk12.5-build

本仓库修改自：https://github.com/lgs2007m/uboot-ipq60xx-build

u-boot-2016 源代码基于：https://github.com/gl-inet/uboot-ipq60xx

**NOR** 机型 U-Boot 仓库：https://github.com/chenxin527/uboot-ipq60xx-nor-build

**NAND** 机型 U-Boot 仓库：https://github.com/chenxin527/uboot-ipq60xx-nand-build

## 适配设备

本项目已适配以下 IPQ60xx **eMMC** 机型：

- 京东云太乙（RE-CS-07）
- 京东云亚瑟（RE-SS-01）
- 京东云雅典娜（RE-CS-02）
- 连我 NN6000（V1 & V2）
- 红米 AX5 JDCloud（RA50）

## 编译方法

### 本地编译

1. 配置编译环境

```bash
# 编译环境：Ubuntu
# mbn 脚本使用 python3 运行，请安装并切换到 python3
sudo apt update
sudo apt install -y python3
sudo apt install -y build-essential device-tree-compiler
```

2. 克隆此仓库

```bash
git clone https://github.com/chenxin527/uboot-ipq60xx-emmc-build.git
```

3. 编译你需要的设备

```
用法: ./build.sh [选项]

选项:
  help                    显示此帮助信息
  setup_env               仅设置编译环境
  check_file_size <文件>  检查并调整文件大小至 640 KB (655360 Bytes)
  clean_cache             清理编译过程中产生的缓存
  build_re-cs-02          编译 JDCloud AX6600 (Athena)
  build_re-cs-07          编译 JDCloud ER1
  build_re-ss-01          编译 JDCloud AX1800 Pro (Arthur)
  build_nn6000            编译 Link NN6000 (V1 & V2)
  build_ax5-jdcloud       编译 Redmi AX5 JDCloud
  build_all               编译所有支持的设备
```

### 云编译

Fork 本仓库后使用 GitHub Actions 云编译。

## 文件说明

编译生成文件所在目录：bin/

日志文件：log-\${编译时间}.txt

U-Boot 文件：uboot-ipq60xx-emmc-\${设备型号}-\${版本号}.bin

U-Boot 截图示例（[点击此处](./screenshots.md) 查看所有网页截图）：

![uboot-index-page](./screenshots/uboot-index-page.png)

## 功能介绍

### 网址说明
| 功能        | 网址                            | 备注                                |
| :---------- | :----------------------------- | :--------------------------------- |
| 更新固件     | http://192.168.1.1             | 支持内核大小为 6MB / 12MB 的 factory / sysupgrade 格式的固件更新 |
| 更新 ART    | http://192.168.1.1/art.html    | ART 包含路由器网卡 MAC 及无线校准数据 |
| 更新 CDT    | http://192.168.1.1/cdt.html    | CDT 文件不得小于 10 KB（10240 Bytes） |
| 更新 IMG    | http://192.168.1.1/img.html    | 支持刷写 eMMC 的 GPT 分区表或完整镜像 |
| 更新 U-Boot | http://192.168.1.1/uboot.html  | U-Boot 大小不能超过 640 KB（655360 Bytes）|
| 启动 uImage | http://192.168.1.1/uimage.html | Initramfs uImage，可直接上传至内存并启动 |

> [!NOTE]
>
> 因 U-Boot HTTP 服务器限制，不支持上传 10 KB（10240 Bytes）以下的文件。若要上传的文件不足 10 KB，请使用十六进制编辑器在文件末尾填充空数据（0x0），但不要超过其所在分区大小。此 U-Boot 支持上传的所有文件中，只有 CDT 文件有效数据不足 10 KB，特此说明。

> [!TIP]
>
> uImage (U-Boot Image) 即所谓 “内存固件”。在 USB 9008 救砖模式下，利用 “启动 uImage” 功能可上传并启动临时 OpenWrt 固件，在临时固件中可使用预置的各种工具进行备份分区、救砖恢复等操作。
>
> [点击此处](http://example.com) 获取 USB 9008 救砖教程及相关文件。
>
> [点击此处](https://github.com/chenxin527/OWRT-BUILDS/releases?q=UIMAGE-ONLY-IPQ60XX&expanded=true) 获取经测试可正常使用的 uImage。

### 进 Web 刷机界面

所有机型都支持通过 RESET 键进入 U-Boot Web 刷机界面。

以下有 WPS 键的机型还支持通过 WPS 键进入 U-Boot Web 刷机界面：

- 京东云亚瑟（原厂叫 JOY 键）
- 京东云雅典娜（原厂叫 JOY 键）
- 连我 NN6000 (V1 & V2)（原厂叫 Reboot 键）

此外，京东云雅典娜还支持通过 SCREEN 键进入 U-Boot Web 刷机界面。

### 其他

**U-Boot 支持 DHCP，无需手动固定 IP（自 [26.01.02-15.33.09](https://github.com/chenxin527/uboot-ipq60xx-emmc-build/releases/tag/26.01.02-15.33.09) 版本起）。** 访问 U-Boot Web 后台时不要将路由器与上级路由连接，以免上级路由 DHCP 干扰。

U-Boot 下不区分 LAN / WAN，任意网口均可进入 Web 刷机界面。

按住 RESET / WPS / SCREEN 键后上电，等待 LED 闪烁 5 次后即可进入 U-Boot Web 刷机界面。

## 注意事项

### 连我 NN6000 V1 的 U-Boot 未测试

连我 NN6000 V1 的 U-Boot 未测试过，因为没有机器。

V1 和 V2 的 U-Boot 只是网口配置不同，其他都一样。若发现 V1 的 U-Boot 不能正常使用，可刷写 V2 的 U-Boot 测试（自 [26.01.02-15.33.09](https://github.com/chenxin527/uboot-ipq60xx-emmc-build/releases/tag/26.01.02-15.33.09) 版本起合并 V1 和 V2 的 U-Boot），看看每个网口是否能正常进 Web。每换一个网口都要断电并重新按 RESET / WPS 键启动 HTTP Server，不要在 HTTP Server 已经启动的时候换网口，这样是进不了 Web 的。

### bootipq 失败后执行 httpd 出错

bootipq 失败后执行 httpd 会出现以下错误：

```
HTTP server is ready!

Data will be downloaded at 0x50000000 in RAM
Upgrade type: firmware
Upload file size: 57282710 bytes
Loading: #######################################
         .......................................
         #######################

done!
data abort
pc : [<4a448310>]          lr : [<4a462c0f>]
reloc pc : [<4a448310>]    lr : [<4a462c0f>]
sp : 4a27f844  ip : 000031b5     fp : 00000a01
r10: 4a487c5c  r9 : 4a27fea0     r8 : 4a487c74
r7 : 4a487c6c  r6 : 0000d250     r5 : 4a48767e  r4 : 0000ba05
r3 : 000005a6  r2 : 4a462c0f     r1 : 000005a6  r0 : 00000000
Flags: nzCv  IRQs off  FIQs off  Mode SVC_32
Resetting CPU ...
```

这是因为 bootipq 命令修改了运行环境，导致执行 httpd 命令刷写固件失败。

若遇到 bootipq 失败的情况，请断电重启路由器以重置运行环境，在 bootipq 执行前按 RESET 键重新进入 Web 界面刷写固件，或者打断 U-Boot 自动启动流程后在串口控制台手动执行相关命令。

bootipq 常见失败原因：U-Boot 无法正常读取 0:HLOS 分区；0:HLOS 分区中存储的不是正确的固件内核数据。
