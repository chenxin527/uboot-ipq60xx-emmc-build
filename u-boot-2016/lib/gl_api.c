#include <common.h>
#include <gl_api.h>
#include <asm/gpio.h>

void led_toggle(unsigned int gpio)
{
	int value;
	value = gpio_get_value(gpio);
	value = !value;
	gpio_set_value(gpio, value);
}

void led_on(unsigned int gpio)
{
	gpio_set_value(gpio, 0);
}

void led_off(unsigned int gpio)
{
	gpio_set_value(gpio, 1);
}

bool button_is_press(unsigned gpio, int value)
{
	if(gpio_get_value(gpio) == value)
	{
		mdelay(10);
		if(gpio_get_value(gpio) == value)
			return true;
		else
			return false;
	}
	else
		return false;
}

void check_button_is_press(void)
{
	int counter = 0;

	while(button_is_press(GPIO_RESET_BTN, GL_RESET_BUTTON_IS_PRESS)){
		
		if(counter == 0)
			printf("Reset button is pressed for: %2d ", counter);
		
		gpio_set_value(GPIO_RED_LED, 0);
		mdelay(350);
		gpio_set_value(GPIO_RED_LED, 1);
		mdelay(350);

		counter++;

		// how long the button is pressed?
		printf("\b\b\b%2d ", counter);

		if(counter >= 5){
			gpio_set_value(GPIO_RED_LED, 0);
			gpio_set_value(GPIO_BLUE_LED, 1);
			run_command("httpd 192.168.1.1", 0);
			run_command("res", 0);
			break;
		}
	}

	if (counter != 0)
		printf("\n");
	
	return;
}

//该函数未调用
int check_test()
{
	int ret = 0;
	volatile unsigned char *f1f = NULL;
	volatile unsigned char *f2i = NULL;
	volatile unsigned char *f3r = NULL;
	volatile unsigned char *f4s = NULL;
	volatile unsigned char *f5t = NULL;
	volatile unsigned char *f6t = NULL;
	volatile unsigned char *f7e = NULL;
	volatile unsigned char *f8s = NULL;
	volatile unsigned char *f9t = NULL;

	volatile unsigned char *s0s = NULL;
	volatile unsigned char *s1e = NULL;
	volatile unsigned char *s2c = NULL;
	volatile unsigned char *s3o = NULL;
	volatile unsigned char *s4n = NULL;
	volatile unsigned char *s5d = NULL;
	volatile unsigned char *s6t = NULL;
	volatile unsigned char *s7e = NULL;
	volatile unsigned char *s8s = NULL;
	volatile unsigned char *s9t = NULL;

	char cmd[128] = {0};
	sprintf(cmd, "sf probe && sf read 0x44000000 0x%x 32",
		(CONFIG_ART_START + 0x50));
	run_command(cmd, 0);

	f1f = (volatile unsigned char *)0x44000000;
	f2i = (volatile unsigned char *)0x44000001;
	f3r = (volatile unsigned char *)0x44000002;
	f4s = (volatile unsigned char *)0x44000003;
	f5t = (volatile unsigned char *)0x44000004;
	f6t = (volatile unsigned char *)0x44000005;
	f7e = (volatile unsigned char *)0x44000006;
	f8s = (volatile unsigned char *)0x44000007;
	f9t = (volatile unsigned char *)0x44000008;

	s0s = (volatile unsigned char *)0x44000010;
	s1e = (volatile unsigned char *)0x44000011;
	s2c = (volatile unsigned char *)0x44000012;
	s3o = (volatile unsigned char *)0x44000013;
	s4n = (volatile unsigned char *)0x44000014;
	s5d = (volatile unsigned char *)0x44000015;
	s6t = (volatile unsigned char *)0x44000016;
	s7e = (volatile unsigned char *)0x44000017;
	s8s = (volatile unsigned char *)0x44000018;
	s9t = (volatile unsigned char *)0x44000019;

	
	if (*f1f==0x66 && *f2i==0x69 && *f3r==0x72 && *f4s==0x73 && \
			*f5t==0x74 && *f6t==0x74 && *f7e==0x65 && *f8s==0x73 && \
			*f9t==0x74 && \
			*s0s==0x73 && *s1e==0x65 && *s2c==0x63 && *s3o==0x6f && \
			*s4n==0x6e && *s5d==0x64 && *s6t==0x74 && *s7e==0x65 && \
			*s8s==0x73 && *s9t==0x74) {
		printf("Device haven tested, checking MAC info...\n");
		ret = 0;
	} else {
		printf("Device haven't tested, please test device in calibration firmware...\n");
		ret = -1;
	}

	return ret;

}

int check_config()
{
	int i = 0;
	u8 addr[6];
	u8 addr_tmp[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	volatile unsigned char *tmp = NULL;

	char cmd[128] = {0};
	sprintf(cmd, "sf read 0x44000000 0x%x 16", 
		CONFIG_ART_START);
	run_command(cmd, 0);

	/*check WAN mac*/
	for (i=0; i<6; i++) {
		tmp = (volatile unsigned char *)0x44000000 + i;
		addr[i] = *tmp;
	}
	//printf("WAN: %x:%x:%x:%x:%x:%x\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!memcmp(addr, addr_tmp, 6)) {
		printf("Device don't have WAN MAC info, please write MAC in calibration firmware...\n");
		return -1;
	}

	/*check LAN mac*/
	for (i=0; i<6; i++) {
		tmp = (volatile unsigned char *)0x44000006 + i;
		addr[i] = *tmp;
	}
	//printf("LAN: %x:%x:%x:%x:%x:%x\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!memcmp(addr, addr_tmp, 6)) {
		printf("Device don't have LAN MAC info, please write MAC in calibration firmware...\n");
		return -1;
	}
	printf("Device have MAC info, starting firmware...\n\n");
	return 0;
}

int check_fw_type(void *address){
	u32 *sign_flas=(u32 *)(address+0x5c);
	u16 *sign_55aa=(u16 *)(address+0x1fe);
	u32 *sign_doodfeed=(u32 *)address;
	u32 *sign_ubi=(u32 *)address;
	u32 *sign_cdt=(u32 *)address;
	u32 *sign_elf=(u32 *)address;

	if (*sign_flas==0x73616c46)
		return FW_TYPE_QSDK;
	else if (*sign_ubi==0x23494255)
		return FW_TYPE_UBI;
	else if (*sign_doodfeed==0xedfe0dd0)
		return FW_TYPE_NOR;
	else if (*sign_55aa==0xaa55)
		return FW_TYPE_EMMC;
	else if (*sign_cdt==0x00544443)
		return FW_TYPE_CDT;
	else if (*sign_elf==0x464c457f)
		return FW_TYPE_ELF;
	else
		return -1;
	return 0;
}
/*

int auto_update_flags = 0;

int check_network(int tryCount)
{
	if (tryCount <= 0) {
		return 1;
	}

	while (tryCount--) {
		if (run_command("ping 192.168.1.2", 0) == 0) {
			auto_update_flags = 1;
			break;
		}
		udelay (1000000);
	}

	if (auto_update_flags) {
		return 0;
	} else {
		return 1;
	}
}

int auto_update_by_tftp()
{
	int ret = -1;
	char buf[128] = {0};
	const char *file_sz_str;
	unsigned long file_size;

	if (check_network(2) != 0) {
		//printf("host no alive.\n");
		return -1;
	}

	if (run_command("tftpboot 0x44000000 openwrt-gl-mv1000.bin", 0) == 0) {
		file_sz_str = getenv("filesize");
		file_size = simple_strtoul(file_sz_str, NULL, 16);
		if(check_fw_type((void *)0x44000000)==FW_TYPE_NOR){
		printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
		sprintf(buf,
				"sf probe && sf update 0x%lx 0x%lx 0x%lx",
				(unsigned long int)0x44000000,
				(unsigned long int)WEBFAILSAFE_UPLOAD_FW_ADDRESS,
				(unsigned long int)file_size);
		}else if(check_fw_type((void *)0x44000000)==FW_TYPE_EMMC){
			printf("\n\n****************************\n*    FIRMWARE UPGRADING    *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			sprintf(buf,
					"mmc dev 0 && mmc erase 0 0x109800 && mmc write 0x%lx 0x%lx 0x%lx",
					(unsigned long int)0x44000000,
					(unsigned long int)0x0,
					(unsigned long int)(file_size/512+1));
		}else{
			return(-1);
		}
		return(run_command(buf, 0));
	}
	return ret;
}

*/
