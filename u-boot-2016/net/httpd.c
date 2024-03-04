/*
 *	Copyright 1994, 1995, 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000, 2001 DENX Software Engineering, Wolfgang Denk, wd@denx.de
 */

#include <common.h>
#include <command.h>
#include <net.h>
#include <asm/byteorder.h>
#include "httpd.h"

#include "../httpd/uipopt.h"
#include "../httpd/uip.h"
#include "../httpd/uip_arp.h"
#include <gl_api.h>
#include <asm/gpio.h>

// extern flash_info_t flash_info[];

static int arptimer = 0;

struct in_addr net_httpd_ip;

// start http daemon
void HttpdStart(void){
	struct uip_eth_addr eaddr;
	unsigned short int ip[2];
	ulong tmp_ip_addr = ntohl(net_ip.s_addr);
	printf( "HTTP server is starting at IP: %ld.%ld.%ld.%ld\n", ( tmp_ip_addr & 0xff000000  ) >> 24, ( tmp_ip_addr & 0x00ff0000  ) >> 16, ( tmp_ip_addr & 0x0000ff00  ) >> 8, ( tmp_ip_addr & 0x000000ff));
	eaddr.addr[0] = net_ethaddr[0];
	eaddr.addr[1] = net_ethaddr[1];
	eaddr.addr[2] = net_ethaddr[2];
	eaddr.addr[3] = net_ethaddr[3];
	eaddr.addr[4] = net_ethaddr[4];
	eaddr.addr[5] = net_ethaddr[5];
	// set MAC address
	uip_setethaddr(eaddr);

	uip_init();
	httpd_init();

	ip[0] =  htons((tmp_ip_addr & 0xFFFF0000) >> 16);
	ip[1] = htons(tmp_ip_addr & 0x0000FFFF);

	uip_sethostaddr(ip);

	printf("done set host addr 0x%x 0x%x\n", uip_hostaddr[0], uip_hostaddr[1]);
	// set network mask (255.255.255.0 -> local network)
	ip[0] = htons((0xFFFFFF00 & 0xFFFF0000) >> 16);
	ip[1] = htons(0xFFFFFF00 & 0x0000FFFF);

	net_netmask.s_addr = 0x00FFFFFF;
	uip_setnetmask(ip);

	// should we also set default router ip address?
	//uip_setdraddr();
	do_http_progress(WEBFAILSAFE_PROGRESS_START);
	webfailsafe_is_running = 1;
}

void HttpdStop(void){
	webfailsafe_is_running = 0;
	webfailsafe_ready_for_upgrade = 0;
	webfailsafe_upgrade_type = WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE;
	do_http_progress(WEBFAILSAFE_PROGRESS_UPGRADE_FAILED);
}
void HttpdDone(void){
	webfailsafe_is_running = 0;
	webfailsafe_ready_for_upgrade = 0;
	webfailsafe_upgrade_type = WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE;
	do_http_progress(WEBFAILSAFE_PROGRESS_UPGRADE_READY);
}

#ifdef CONFIG_MD5
#include <u-boot/md5.h>

void printChecksumMd5(int address,unsigned int size)
{
	void *buf = (void*)(address);
	int i = 0;
	u8 output[16];
	md5_wd(buf, size, output, CHUNKSZ_MD5);
	printf("md5 for %08x ... %08x ==> ", address, address + size);
	for (i = 0; i < 16; i++)
		printf("%02x", output[i] & 0xFF);
	printf("\n");
}
#else
void printChecksumMd5(int address,unsigned int size)
{
}
#endif

int do_http_upgrade(const ulong size, const int upgrade_type){
	//char buf[96];
	char buf[576];
	//为了能加入更多命令，加大的了buf
	//printf checksum if defined
	printChecksumMd5(WEBFAILSAFE_UPLOAD_RAM_ADDRESS,size);

	if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_UBOOT){
		printf("\n\n****************************\n*     U-BOOT UPGRADING     *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		//arch/arm/include/asm/arch-qca-common/smem.h
		//SMEM_BOOT_NO_FLASH        = 0,
		//SMEM_BOOT_NOR_FLASH       = 1,
		//SMEM_BOOT_NAND_FLASH      = 2,
		//SMEM_BOOT_ONENAND_FLASH   = 3,
		//SMEM_BOOT_SDC_FLASH       = 4,
		//SMEM_BOOT_MMC_FLASH       = 5,
		//SMEM_BOOT_SPI_FLASH       = 6,
		//SMEM_BOOT_NORPLUSNAND     = 7,
		//SMEM_BOOT_NORPLUSEMMC     = 8,
		if(qca_smem_flash_info.flash_type==5){
			//mw 0x%lx 0x00 0x200 擦除内存中上传文件后面的512字节，防止文件不够512字节写入文件后其他字符到eMMC
			//其实测试不擦除文件后内存，写入一些其他字符也可以正常启动
			sprintf(buf,"mmc dev 0 && mw 0x%lx 0x00 0x200 && flash 0:APPSBL && flash 0:APPSBL_1",
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+size));
		}else if(qca_smem_flash_info.flash_type==2){
			sprintf(buf,
				"nand erase 0x%lx 0x%lx; nand write 0x%lx 0x%lx 0x%lx",
				(unsigned long int)WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS_NAND,
				(unsigned long int)WEBFAILSAFE_UPLOAD_UBOOT_SIZE_IN_BYTES_NAND,
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS_NAND,
				(unsigned long int)((size/131072+(size%131072!=0))*131072));
		}else if(qca_smem_flash_info.flash_type==6){
			sprintf(buf,
				"sf probe && sf update 0x%lx 0x%lx 0x%lx",
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS,
				(unsigned long int)size);
		}
	} else if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE){
		//include/gl_api.h
		//FW_TYPE_NOR	0 这个是刷写固件分区，factory固件
		//FW_TYPE_EMMC	1 这个是刷写eMMC镜像，不是刷写固件分区
		//FW_TYPE_QSDK	2 这个官方原厂固件img
		//FW_TYPE_UBI	3
		if(check_fw_type((void *)WEBFAILSAFE_UPLOAD_RAM_ADDRESS)==FW_TYPE_NOR){
			//固件在nor的情况，不会发生
			printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			sprintf(buf,"mmc dev 0 && mw 0x%lx 0x00 0x200 && flash 0:HLOS 0x%lx 0x%lx && flash rootfs 0x%lx 0x%lx && mmc read 0x%lx 0x622 0x200 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && flash 0:BOOTCONFIG 0x%lx 0x40000 && flash 0:BOOTCONFIG1 0x%lx 0x40000",
				//mw 0x%lx 0x00 0x200 擦除内存中上传文件后面的512字节，防止文件不够512字节写入文件后其他字符到eMMC
				//其实测试不擦除文件后内存，写入一些其他字符也可以正常启动
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+size),
				//factory.bin由kernel+rootfs组成，其中kernel固定6MB大小
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)0x600000,
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x600000),
				(unsigned long int)(size-0x600000),
				//这部分改两个BOOTCONFIG，启动系统0，即rootfs
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x80),
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x94),
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0xA8),
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);
			//sprintf(buf,
				//"sf probe && sf update 0x%lx 0x%lx 0x%lx",
				//(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				//(unsigned long int)WEBFAILSAFE_UPLOAD_FW_ADDRESS,
				//(unsigned long int)size);
		}else if(check_fw_type((void *)WEBFAILSAFE_UPLOAD_RAM_ADDRESS)==FW_TYPE_EMMC){
			//固件为emmc mbr分区的情况，不会发生
			printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			printf("\n\n* DO NOT SUPPORT eMMC IMAGE!! *\n\n");
			return(-1);
			//sprintf(buf,
				//"mmc dev 0 && mmc erase 0 0x109800 && mmc write 0x%lx 0x%lx 0x%lx",
				//(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				//(unsigned long int)0x0,
				//(unsigned long int)(size/512+1));
		}else if(check_fw_type((void *)WEBFAILSAFE_UPLOAD_RAM_ADDRESS)==FW_TYPE_QSDK){
			printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			sprintf(buf,"mmc dev 0 && imxtract 0x%lx hlos-0cc33b23252699d495d79a843032498bfa593aba && flash 0:HLOS $fileaddr $filesize && imxtract 0x%lx rootfs-f3c50b484767661151cfb641e2622703e45020fe && flash rootfs $fileaddr $filesize && imxtract 0x%lx wififw-45b62ade000c18bfeeb23ae30e5a6811eac05e2f && flash 0:WIFIFW $fileaddr $filesize && mmc read 0x%lx 0x622 0x200 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && flash 0:BOOTCONFIG 0x%lx 0x40000 && flash 0:BOOTCONFIG1 0x%lx 0x40000",
				//官方固件本身各个固件后面有填充0，所以不用修改上传文件后的内存
				//执行imxtract时不带目标地址，则不进行复制，但会修改环境变量$fileaddr $filesize，可以直接用
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				//这部分改两个BOOTCONFIG，启动系统0，即rootfs
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x80),
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x94),
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0xA8),
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);
			//*sprintf(buf, "sf probe; imgaddr=0x%lx && source $imgaddr:script", (unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);
		}else if(check_fw_type((void *)WEBFAILSAFE_UPLOAD_RAM_ADDRESS)==FW_TYPE_UBI){
			printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			sprintf(buf, "nand erase 0xa00000 0x7300000; nand write 0x%lx 0xa00000 0x%lx", (unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS, (unsigned long int)size);
		}else {
			return(-1);
		}
	} else if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_ART){
		// TODO: add option to change ART partition offset,
		// for those who want to use OFW on router with replaced/bigger FLASH
		printf("\n\n****************************\n*      ART  UPGRADING      *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		if(qca_smem_flash_info.flash_type==5){
			sprintf(buf,"mmc dev 0 && mw 0x%lx 0x00 0x200 && flash 0:ART",
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+size));
		}else if(qca_smem_flash_info.flash_type==2){
			sprintf(buf,
				"nand erase 0x%lx 0x%lx; nand write 0x%lx 0x%lx 0x%lx",
				(unsigned long int)WEBFAILSAFE_UPLOAD_ART_ADDRESS_NAND,
				(unsigned long int)WEBFAILSAFE_UPLOAD_ART_SIZE_IN_BYTES_NAND,
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_ART_ADDRESS_NAND,
				(unsigned long int)((size/131072+(size%131072!=0))*131072));
		}else if(qca_smem_flash_info.flash_type==6){
			sprintf(buf,
				"sf probe && sf update 0x%lx 0x%lx 0x%lx",
				(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
				(unsigned long int)WEBFAILSAFE_UPLOAD_ART_ADDRESS,
				(unsigned long int)size);
		}
	} else if(upgrade_type == WEBFAILSAFE_UPGRADE_TYPE_GPT){
		printf("\n\n****************************\n*      GPT  UPGRADING      *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		if(qca_smem_flash_info.flash_type==5){
			sprintf(buf,"mmc dev 0 && mmc erase 0x0 0x22 && mmc write 0x%lx 0x0 0x22",
				(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS));
		}else {
			printf("\n\n* ONLY eMMC DEVICE CAN UPGRADE GPT!! *\n\n");
			return(-1);
		}
	}
	else {
		return(-1);
	}

	printf("Executing: %s\n\n", buf);
	return(run_command(buf, 0));
}

// info about current progress of failsafe mode
int do_http_progress(const int state){
	//unsigned char i = 0;

	/* toggle LED's here */
	switch(state){
		case WEBFAILSAFE_PROGRESS_START:

			// // blink LED fast 10 times
			// for(i = 0; i < 10; ++i){
			// 	all_led_on();
			// 	milisecdelay(25);
			// 	all_led_off();
			// 	milisecdelay(25);
			// }
			gpio_set_value(GPIO_RED_LED, LED_OFF);
			gpio_set_value(GPIO_GREEN_LED, LED_OFF);
			gpio_set_value(GPIO_BLUE_LED, LED_ON);
			printf("HTTP server is ready!\n\n");
			break;

		case WEBFAILSAFE_PROGRESS_TIMEOUT:
			//printf("Waiting for request...\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPLOAD_READY:
			printf("HTTP upload is done! Upgrading...\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPGRADE_READY:
			gpio_set_value(GPIO_RED_LED, LED_OFF);
			gpio_set_value(GPIO_GREEN_LED, LED_ON);
			gpio_set_value(GPIO_BLUE_LED, LED_OFF);
			printf("HTTP ugrade is done! Rebooting...\n\n");
			mdelay(3000);
			break;

		case WEBFAILSAFE_PROGRESS_UPGRADE_FAILED:
			printf("## Error: HTTP ugrade failed!\n\n");

			// // blink LED fast for 4 sec
			// for(i = 0; i < 80; ++i){
			// 	all_led_on();
			// 	milisecdelay(25);
			// 	all_led_off();
			// 	milisecdelay(25);
			// }

			// wait 1 sec
			// milisecdelay(1000);

			break;
	}

	return(0);
}

void NetSendHttpd(void){

	volatile uchar *tmpbuf = net_tx_packet;
	int i;

	for(i = 0; i < 40 + UIP_LLH_LEN; i++){

		tmpbuf[i] = uip_buf[i];
	}

	for(; i < uip_len; i++){

		tmpbuf[i] = uip_appdata[i - 40 - UIP_LLH_LEN];
	}

	eth_send(net_tx_packet, uip_len);
}

void NetReceiveHttpd(volatile uchar * inpkt, int len){

	memcpy(uip_buf, (const void *)inpkt, len);
	uip_len = len;
	struct uip_eth_hdr * tmp = (struct uip_eth_hdr *)&uip_buf[0];
	if(tmp->type == htons(UIP_ETHTYPE_IP)){

		uip_arp_ipin();
		uip_input();

		if(uip_len > 0){

			uip_arp_out();
			NetSendHttpd();
		}
	} else if(tmp->type == htons(UIP_ETHTYPE_ARP)){

		uip_arp_arpin();

		if(uip_len > 0){

			NetSendHttpd();
		}
	}
}

void HttpdHandler(void){
	int i;

	for(i = 0; i < UIP_CONNS; i++){
		uip_periodic(i);

		if(uip_len > 0){
			uip_arp_out();
			NetSendHttpd();
		}
	}

	if(++arptimer == 20){
		uip_arp_timer();
		arptimer = 0;
	}
}
