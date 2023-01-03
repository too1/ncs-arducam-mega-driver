/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <string.h>
#include <arducam_mega.h>

const struct device *camera_device;

void main(void)
{
	int err;

	printk("Arducam Mega test example started\n");

	k_msleep(1000);

	camera_device = device_get_binding(DT_NODE_FULL_NAME(DT_NODELABEL(arducam_mega)));
	if(camera_device == 0) {
		printk("ERROR: Camera device not found!\n");
		return;
	}
	
	while(1) {
		printk("Attempting to take picture...\n");
		arducam_mega_take_picture(camera_device, CAM_IMAGE_MODE_QVGA, CAM_IMAGE_PIX_FMT_JPG);
		uint8_t data_buff[240];
		uint8_t bytes_read;
		int totalbytes = 0;
		do {
			bytes_read = arducam_mega_read_image_buf(camera_device, data_buff, 240);
			totalbytes += bytes_read;
			printk("*");
		}while(bytes_read > 0);
		printk("\nImage readout finished. %i bytes total\n", totalbytes);
		k_msleep(2000);
	}
}
