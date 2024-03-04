You will also like to see https://github.com/lgs2007m/cdt-gpt-ipq60xx-generate

## uboot build 
u-boot-2016 source code base on https://github.com/gl-inet/uboot-ipq60xx

PS: The mbn scripts run with python2.7, you should install and switch to python2.7.
```
git clone https://github.com/lgs2007m/uboot-ipq60xx-build
cd uboot-ipq60xx-build/
./build.sh
```

The uboot binary will be: uboot-ipq60xx-build/u-boot.bin

The uboot failsafe HTTP server ip: 192.168.1.1  
Update firmware: http://192.168.1.1  
Update ART: http://192.168.1.1/art.html  
Update GPT: http://192.168.1.1/gpt.html  
Update U-BOOT: http://192.168.1.1/uboot.html  
