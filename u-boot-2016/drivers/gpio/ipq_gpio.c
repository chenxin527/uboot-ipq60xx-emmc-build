/*
 * Copyright (c) 2016-2017 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <asm/io.h>
#include <common.h>
#include <asm/types.h>
#include <fdtdec.h>
#include <asm/arch-qca-common/gpio.h>
#include <gl_api.h>

DECLARE_GLOBAL_DATA_PTR;

/***********************************************************
* Function description: configure GPIO functinality
* Arguments :
* struct qca_gpio_config gpio_config - GPIO Configuration bits
*
* Return : None
************************************************************/
void gpio_tlmm_config(struct qca_gpio_config *gpio_config)
{
	unsigned int val = 0;
	val |= gpio_config->pull;
	val |= gpio_config->func << 2;
	val |= gpio_config->drvstr << 6;
	val |= gpio_config->oe << 9;
	val |= gpio_config->vm << 11;
	val |= gpio_config->od_en << 12;
	val |= gpio_config->pu_res << 13;

	unsigned int *addr =
		(unsigned int *)GPIO_CONFIG_ADDR(gpio_config->gpio);
	writel(val, addr);

	/* Output value is only relevant if GPIO has been configured for fixed
	 * output setting - i.e. func == 0
	 */
	if (gpio_config->func == 0) {
		addr = (unsigned int *)GPIO_IN_OUT_ADDR(gpio_config->gpio);
		val = readl(addr);
		val |= gpio_config->out << 1;
		writel(val, addr);
	}

	return;
}

void gpio_set_value(unsigned int gpio, unsigned int out)
{
	unsigned int *addr = (unsigned int *)GPIO_IN_OUT_ADDR(gpio);
	unsigned int val = 0;

	val = readl(addr);
	val &= ~(0x2);
	val |= out << 1;
	writel(val, addr);
}

int gpio_get_value(unsigned int gpio)
{
	unsigned int *addr = (unsigned int *)GPIO_IN_OUT_ADDR(gpio);
	unsigned int val = readl(addr);

	return (val & 1);
}

void gpio_direction_output(unsigned int gpio, unsigned int out)
{
	unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);
	unsigned int val = 0;

	gpio_set_value(gpio, out);
	val = readl(addr);
	val |= 1 << 9;
	writel(val, addr);
}

int qca_gpio_init(int offset)
{
	struct qca_gpio_config gpio_config;

	for (offset = fdt_first_subnode(gd->fdt_blob, offset); offset > 0;
	     offset = fdt_next_subnode(gd->fdt_blob, offset)) {

		gpio_config.gpio	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "gpio", 0);
		gpio_config.func	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "func", 0);
		gpio_config.out		= fdtdec_get_uint(gd->fdt_blob,
							  offset, "out", 0);
		gpio_config.pull	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "pull", 0);
		gpio_config.drvstr	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "drvstr", 0);
		gpio_config.oe		= fdtdec_get_uint(gd->fdt_blob,
							  offset, "oe", 0);
		gpio_config.vm		= fdtdec_get_uint(gd->fdt_blob,
							  offset, "vm", 0);
		gpio_config.od_en	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "od_en", 0);
		gpio_config.pu_res	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "pu_res", 0);

		gpio_tlmm_config(&gpio_config);
	}
	return 0;
}

int qca_gpio_deinit(int offset)
{
	struct qca_gpio_config gpio_config;

	for (offset = fdt_first_subnode(gd->fdt_blob, offset); offset > 0;
	     offset = fdt_next_subnode(gd->fdt_blob, offset)) {

		gpio_config.gpio	= fdtdec_get_uint(gd->fdt_blob,
							  offset, "gpio", 0);
		unsigned int *addr =
			(unsigned int *)GPIO_CONFIG_ADDR(gpio_config.gpio);
		writel(1, addr);
		addr = (unsigned int *)GPIO_IN_OUT_ADDR(gpio_config.gpio);
		writel(1, addr);
	}
	return 0;
}

void gl_led_booting(void)
{
	//gpio_set_value(GPIO_RED_LED, 0x0);
	//gpio_set_value(GPIO_GREEN_LED, 0x0);
	//gpio_set_value(GPIO_BLUE_LED, 0x0);
}

void gl_gpio_init(const char *gpio_name)
{
	int node;
	struct qca_gpio_config gpio_config;
	
	node = fdt_path_offset(gd->fdt_blob, gpio_name);
	if (node < 0) {
		printf("Could not find %s node\n", gpio_name);
		return;
	}
	
	gpio_config.gpio	= fdtdec_get_uint(gd->fdt_blob,
						  node, "gpio", 0);
	gpio_config.func	= fdtdec_get_uint(gd->fdt_blob,
						  node, "func", 0);
	gpio_config.out		= fdtdec_get_uint(gd->fdt_blob,
						  node, "out", 0);
	gpio_config.pull	= fdtdec_get_uint(gd->fdt_blob,
						  node, "pull", 0);
	gpio_config.drvstr	= fdtdec_get_uint(gd->fdt_blob,
						  node, "drvstr", 0);
	gpio_config.oe		= fdtdec_get_uint(gd->fdt_blob,
						  node, "oe", 0);
	gpio_config.vm		= fdtdec_get_uint(gd->fdt_blob,
						  node, "vm", 0);
	gpio_config.od_en	= fdtdec_get_uint(gd->fdt_blob,
						  node, "od_en", 0);
	gpio_config.pu_res	= fdtdec_get_uint(gd->fdt_blob,
						  node, "pu_res", 0);

	gpio_tlmm_config(&gpio_config);
}

void gl_led_init(void)
{
	gl_gpio_init("power_led");
	gl_gpio_init("blink_led");
	gl_gpio_init("system_led");
	
	led_on("power_led");
	led_on("blink_led");
	led_on("system_led");
	mdelay(1000);
	led_off("blink_led");
	led_off("system_led");
	
	/* unsigned int *gpio_base;

	gpio_base = (unsigned int *)GPIO_CONFIG_ADDR(GPIO_RED_LED);
	writel(0x203, gpio_base);
	gpio_direction_output(GPIO_RED_LED, 0x1);
	
	gpio_base = (unsigned int *)GPIO_CONFIG_ADDR(GPIO_GREEN_LED);
	writel(0x203, gpio_base);
	gpio_direction_output(GPIO_GREEN_LED, 0x1);

	gpio_base = (unsigned int *)GPIO_CONFIG_ADDR(GPIO_BLUE_LED);
	writel(0x203, gpio_base);
	gpio_direction_output(GPIO_BLUE_LED, 0x1);

	gpio_set_value(GPIO_RED_LED, 0x1);
	gpio_set_value(GPIO_GREEN_LED, 0x1);
	gpio_set_value(GPIO_BLUE_LED, 0x1);
	mdelay(1000);
	gpio_set_value(GPIO_GREEN_LED, 0x0);
	gpio_set_value(GPIO_BLUE_LED, 0x0);
	*/
}

void gl_btn_init(void)
{
	gl_gpio_init("reset_key");
	
	/*unsigned int *gpio_base;

	gpio_base = (unsigned int *)GPIO_CONFIG_ADDR(GPIO_JOYLINK_BTN);
	writel(0xc3, gpio_base);

	gpio_base = (unsigned int *)GPIO_CONFIG_ADDR(GPIO_RESET_BTN);
	writel(0xc3, gpio_base);
	*/
}
