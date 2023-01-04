/*
 * Copyright (c) 2016 Intel Corporation.
 * Copyright (c) 2019-2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/fs/fs.h>
#include <stdio.h>
#include <arducam_mega.h>

LOG_MODULE_REGISTER(main);

#if CONFIG_DISK_DRIVER_FLASH
#include <zephyr/storage/flash_map.h>
#endif

#if CONFIG_FAT_FILESYSTEM_ELM
#include <ff.h>
#endif

#if CONFIG_FILE_SYSTEM_LITTLEFS
#include <zephyr/fs/littlefs.h>
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
#endif

#define STORAGE_PARTITION		storage_partition
#define STORAGE_PARTITION_ID		FIXED_PARTITION_ID(STORAGE_PARTITION)

static struct fs_mount_t fs_mnt;

const struct device *dev_camera;

static int setup_flash(struct fs_mount_t *mnt)
{
	int rc = 0;
#if CONFIG_DISK_DRIVER_FLASH
	unsigned int id;
	const struct flash_area *pfa;

	mnt->storage_dev = (void *)STORAGE_PARTITION_ID;
	id = STORAGE_PARTITION_ID;

	rc = flash_area_open(id, &pfa);
	printk("Area %u at 0x%x on %s for %u bytes\n",
	       id, (unsigned int)pfa->fa_off, pfa->fa_dev->name,
	       (unsigned int)pfa->fa_size);

	if (rc < 0 && IS_ENABLED(CONFIG_APP_WIPE_STORAGE)) {
		printk("Erasing flash area ... ");
		rc = flash_area_erase(pfa, 0, pfa->fa_size);
		printk("%d\n", rc);
	}

	if (rc < 0) {
		flash_area_close(pfa);
	}
#endif
	return rc;
}

static int mount_app_fs(struct fs_mount_t *mnt)
{
	int rc;

#if CONFIG_FAT_FILESYSTEM_ELM
	static FATFS fat_fs;

	mnt->type = FS_FATFS;
	mnt->fs_data = &fat_fs;
	if (IS_ENABLED(CONFIG_DISK_DRIVER_RAM)) {
		mnt->mnt_point = "/RAM:";
	} else if (IS_ENABLED(CONFIG_DISK_DRIVER_SDMMC)) {
		mnt->mnt_point = "/SD:";
	} else {
		mnt->mnt_point = "/NAND:";
	}

#elif CONFIG_FILE_SYSTEM_LITTLEFS
	mnt->type = FS_LITTLEFS;
	mnt->mnt_point = "/lfs";
	mnt->fs_data = &storage;
#endif
	rc = fs_mount(mnt);

	return rc;
}

static void setup_disk(void)
{
	struct fs_mount_t *mp = &fs_mnt;
	struct fs_dir_t dir;
	struct fs_statvfs sbuf;
	int rc;

	fs_dir_t_init(&dir);

	if (IS_ENABLED(CONFIG_DISK_DRIVER_FLASH)) {
		rc = setup_flash(mp);
		if (rc < 0) {
			LOG_ERR("Failed to setup flash area");
			return;
		}
	}

	if (!IS_ENABLED(CONFIG_FILE_SYSTEM_LITTLEFS) &&
	    !IS_ENABLED(CONFIG_FAT_FILESYSTEM_ELM)) {
		LOG_INF("No file system selected");
		return;
	}

	rc = mount_app_fs(mp);
	if (rc < 0) {
		LOG_ERR("Failed to mount filesystem");
		return;
	}

	/* Allow log messages to flush to avoid interleaved output */
	k_sleep(K_MSEC(50));

	printk("Mount %s: %d\n", fs_mnt.mnt_point, rc);

	rc = fs_statvfs(mp->mnt_point, &sbuf);
	if (rc < 0) {
		printk("FAIL: statvfs: %d\n", rc);
		return;
	}

	printk("%s: bsize = %lu ; frsize = %lu ;"
	       " blocks = %lu ; bfree = %lu\n",
	       mp->mnt_point,
	       sbuf.f_bsize, sbuf.f_frsize,
	       sbuf.f_blocks, sbuf.f_bfree);

	rc = fs_opendir(&dir, mp->mnt_point);
	printk("%s opendir: %d\n", mp->mnt_point, rc);

	if (rc < 0) {
		LOG_ERR("Failed to open directory");
	}

	while (rc >= 0) {
		struct fs_dirent ent = { 0 };

		rc = fs_readdir(&dir, &ent);
		if (rc < 0) {
			LOG_ERR("Failed to read directory entries");
			break;
		}
		if (ent.name[0] == 0) {
			printk("End of files\n");
			break;
		}
		printk("  %c %u %s\n",
		       (ent.type == FS_DIR_ENTRY_FILE) ? 'F' : 'D',
		       ent.size,
		       ent.name);
	}

	(void)fs_closedir(&dir);

	return;
}

static void img_find_free_file_name(uint8_t *out_ptr)
{
	int ret;
	uint8_t tmp_file_name[64];
	struct fs_file_t file;
	fs_file_t_init(&file);
	for(int index = 0; index < 1000; index++) {
		sprintf(tmp_file_name, "%s/image%03i.jpg", fs_mnt.mnt_point, index);
		ret = fs_open(&file, tmp_file_name, FS_O_READ);
		if(ret == 0) {
			fs_close(&file);
			continue;
		}
		// fs_open error, assume the file doesn't exist
		strcpy(out_ptr, tmp_file_name);
		return;
	}
}

void main(void)
{
	int ret;

	printk("Camera Mass Storage Example started\n");

	dev_camera = device_get_binding(DT_NODE_FULL_NAME(DT_NODELABEL(arducam_mega)));
	if(dev_camera == 0) {
		printk("ERROR: Camera device not found!\n");
		return;
	}

	setup_disk();

	for(int i = 0; i < 4; i++) {
		k_msleep(1000);
		
		// Take a number of pictures and store to the flash, before enabling the USB interface to allow 
		// reading the images over USB
		uint8_t filename[64];
		struct fs_file_t img_file;

		// Find a free file name 
		img_find_free_file_name(filename);

		// Have the camera take a picture
		arducam_mega_take_picture(dev_camera, CAM_IMAGE_MODE_VGA, CAM_IMAGE_PIX_FMT_JPG);

		// Open a new file for writing the image data into
		printk("Creating new image file: %s\n", filename);
		fs_file_t_init(&img_file);
		if(fs_open(&img_file, filename, FS_O_WRITE | FS_O_CREATE) < 0) {
			printk("File open error. Aborting\n");
			return;
		}

		// Read the image data from the camera 240 bytes at a time and write it to the file
		uint8_t data_buff[240];
		int bytes_read;
		int bytes_total = 0;
		do {
			bytes_read = arducam_mega_read_image_buf(dev_camera, data_buff, 240);
			if(fs_write(&img_file, data_buff, bytes_read) < 0) {
				printk("File write error. Aborting\n");
				return;
			}
			bytes_total += bytes_read;
			printk("\r%i", bytes_total);
		}while(bytes_read > 0);

		printk("\nImage stored to file\n");

		// Close the file
		fs_close(&img_file);
	}

	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return;
	}

	LOG_INF("The device is put in USB mass storage mode.\n");
}
