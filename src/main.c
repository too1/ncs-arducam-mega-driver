/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <string.h>

void arducam_mega_evt_handler(struct arducam_mega_evt_t *evt)
{

}

void main(void)
{
	int err;

	printk("Arducam Mega test example started\n");

	arducam_mega_init(arducam_mega_evt_handler);
	
	while(1) {
		k_msleep(100);
	}
}
