#!/bin/sh

if [ "$1" = "clean_all" ]; then
	echo "Delete old u-boot files"
	# Only delete the specified core files
	if [ -f "openwrt-ipq6018-u-boot.mbn" ]; then
		rm openwrt-ipq6018-u-boot.mbn
	fi
	exit 0
elif [ "$1" = "clean" ]; then
	echo "Delete old u-boot files"
	# Keep existing clean logic with all deletion rules
	if [ -f "openwrt-ipq6018-u-boot.mbn" ]; then
		rm openwrt-ipq6018-u-boot.mbn
	fi
	echo "Deep clean by .gitignore rules"
	find u-boot-2016 -type f \
		\( \
			-name '*.o' -o \
			-name '*.o.*' -o \
			-name '*.a' -o \
			-name '*.s' -o \
			-name '*.su' -o \
			-name '*.mod.c' -o \
			-name '*.i' -o \
			-name '*.lst' -o \
			-name '*.order' -o \
			-name '*.elf' -o \
			-name '*.swp' -o \
			-name '*.bin' -o \
			-name '*.patch' -o \
			-name '*.cfgtmp' -o \
			-name '*.exe' -o \
			-name 'MLO*' -o \
			-name 'SPL' -o \
			-name 'System.map' -o \
			-name 'LOG' -o \
			-name '*.orig' -o \
			-name '*~' -o \
			-name '#*#' -o \
			-name 'cscope.*' -o \
			-name 'tags' -o \
			-name 'ctags' -o \
			-name 'etags' -o \
			-name 'GPATH' -o \
			-name 'GRTAGS' -o \
			-name 'GSYMS' -o \
			-name 'GTAGS' \
		\) -delete
	rm -rf \
		.stgit-edit.txt \
		.gdb_history \
		u-boot-2016/arch/arm/dts/dtbtable.S \
		u-boot-2016/httpd/fsdata.c \
		u-boot-2016/scripts_mbn/mbn_tools.pyc \
		u-boot-2016/u-boot*
	exit 0
fi

echo "Set Compilation Environment"
cd u-boot-2016/
. ../env.sh

echo "Build ipq6018"
make ipq6018_defconfig
# make menuconfig
make V=s

echo "Strip elf"
arm-openwrt-linux-strip u-boot

echo "Convert elf to mbn"
python2.7 scripts_mbn/elftombn.py -f ./u-boot -o ./u-boot.mbn -v 6

echo "Copy u-boot.mbn to root directory"
mv ./u-boot.mbn ../openwrt-ipq6018-u-boot.mbn

echo "Done!"
