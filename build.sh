#!/bin/sh

echo "Set Compilation Environment"
cd u-boot-2016/
. ../env.sh

echo "Build"
make ipq6018_defconfig
# make menuconfig
make V=s

echo "Strip elf"
arm-openwrt-linux-strip u-boot

echo "Convert elf to mbn"
python2.7 scripts_mbn/elftombn.py -f ./u-boot -o ./u-boot.mbn -v 6

echo "Copy u-boot.mbn to root directory"
mv ./u-boot.mbn ../u-boot.bin

echo "Done!"
