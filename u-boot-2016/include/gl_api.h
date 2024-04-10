//#define GPIO_RED_LED 37
//#define GPIO_GREEN_LED 50
//#define GPIO_BLUE_LED 35
//#define GPIO_RESET_BTN 9
//#define GPIO_JOYLINK_BTN 8

#define GL_RESET_BUTTON_IS_PRESS        0

#define LED_ON 1
#define LED_OFF 0

void check_button_is_press(void);

#define WEBFAILSAFE_PROGRESS_START		0
#define WEBFAILSAFE_PROGRESS_TIMEOUT		1
#define WEBFAILSAFE_PROGRESS_UPLOAD_READY	2
#define WEBFAILSAFE_PROGRESS_UPGRADE_READY	3
#define WEBFAILSAFE_PROGRESS_UPGRADE_FAILED	4

#define WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE	0
#define WEBFAILSAFE_UPGRADE_TYPE_UBOOT		1
#define WEBFAILSAFE_UPGRADE_TYPE_ART		2
#define WEBFAILSAFE_UPGRADE_TYPE_IMG		3
#define WEBFAILSAFE_UPGRADE_TYPE_CDT		4

#define CONFIG_LOADADDR                                 0x44000000
#define WEBFAILSAFE_UPLOAD_RAM_ADDRESS                  0x50000000

#define WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS                0x520000
#define WEBFAILSAFE_UPLOAD_ART_ADDRESS                  0x660000
#define WEBFAILSAFE_UPLOAD_FW_ADDRESS                   0x6a0000 //单nor情况,不会发生

#define WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS_NAND           0x800000
#define WEBFAILSAFE_UPLOAD_ART_ADDRESS_NAND             0x980000
#define WEBFAILSAFE_UPLOAD_FW_ADDRESS_NAND              0xa00000

#define CONFIG_ART_START                  0x660000

#define WEBFAILSAFE_UPLOAD_UBOOT_SIZE_IN_BYTES          (640*1024)
#define WEBFAILSAFE_UPLOAD_ART_SIZE_IN_BYTES            (256*1024)
#define WEBFAILSAFE_UPLOAD_ART_BIG_SIZE_IN_BYTES        (512*1024)
#define WEBFAILSAFE_UPLOAD_CDT_SIZE_IN_BYTES            (256*1024)

#define WEBFAILSAFE_UPLOAD_UBOOT_SIZE_IN_BYTES_NAND     (1536*1024)
#define WEBFAILSAFE_UPLOAD_ART_SIZE_IN_BYTES_NAND       (512*1024)

#define FW_TYPE_NOR 0
#define FW_TYPE_EMMC 1
#define FW_TYPE_QSDK 2
#define FW_TYPE_UBI 3
#define FW_TYPE_CDT 4
#define FW_TYPE_ELF 5

int check_test(void);
int check_config(void);
int auto_update_by_tftp(void);
int check_fw_type(void *address);
void led_toggle(const char *gpio_name);
void led_on(const char *gpio_name);
void led_off(const char *gpio_name);
