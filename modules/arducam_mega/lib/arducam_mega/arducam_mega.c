#define DT_DRV_COMPAT nordic_arducam_mega

#include <arducam_mega.h>
#include <ArducamCamera.h>
#include <zephyr/sys/ring_buffer.h>
#include <string.h>

#include <zephyr/logging/log.h>

#define LOG_MODULE_NAME arducam_mega
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

static int arducam_mega_int_init(const struct device *dev)
{
	const struct arducam_mega_cfg *const config = dev->config;
    struct arducam_mega_data *data = dev->data;

    data->bus = device_get_binding (config->bus_name);
    if (!data->bus) {
        LOG_WRN ("SPI master not found: %s", config->bus_name);
        return -EINVAL;
    }
	data->camera = createArducamCamera(data->bus);
	data->camera.spi_cs_gpio_spec = &data->cs_ctrl.gpio;
	begin(&data->camera);
	LOG_DBG("Arducam Mega driver initialized");
	return 0;
}

int arducam_mega_take_picture(const struct device *dev, CAM_IMAGE_MODE mode, CAM_IMAGE_PIX_FMT pixel_format)
{
	ArducamCamera *camera = &((struct arducam_mega_data *)dev->data)->camera;
	takePicture(camera, mode, pixel_format);
	return 0;
}

uint8_t arducam_mega_read_image_buf(const struct device *dev, uint8_t* buff, uint8_t length)
{
	ArducamCamera *camera = &((struct arducam_mega_data *)dev->data)->camera;
	return readBuff(camera, buff, length);	
}

void uart_event_thread_func(void)
{
	static struct arducam_mega_evt_t new_event;
}

K_THREAD_DEFINE(app_uart_evt_thread, CONFIG_ARDUCAM_MEGA_EVT_THREAD_STACK_SIZE, uart_event_thread_func, \
				0, 0, 0, CONFIG_ARDUCAM_MEGA_EVT_THREAD_PRIORITY, 0, 100);

#define ARDUCAM_MEGA_DATA(inst)                                                                                                               \
	{                                                                                                                                       \
		.cs_ctrl = {                                                                                                                    \
			.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_INST(0, nordic_arducam_mega)),	\
			.delay = 10 \
		},                                                                                                                              \
	}

#define ARDUCAM_MEGA_CONFIG(inst)           \
	{										\
		.bus_name = DT_INST_BUS_LABEL(inst),			\
		.spi_conf = {						\
			.frequency =					\
				DT_INST_PROP(inst, spi_max_frequency),	\
			.operation = (SPI_WORD_SET(8) |	SPI_OP_MODE_MASTER), \
			.slave = DT_INST_REG_ADDR(inst),		\
			.cs = &(arducam_mega_data_##inst.cs_ctrl),			\
		},												\
		.cs_gpios_label = 0 /*DT_INST_SPI_DEV_CS_GPIOS_LABEL (inst),*/		\
	}

#define ARDUCAM_MEGA_DEFINE(inst) \
	static struct arducam_mega_data arducam_mega_data_##inst = ARDUCAM_MEGA_DATA(inst);                                                               \
	static const struct arducam_mega_cfg arducam_mega_config_##inst = ARDUCAM_MEGA_CONFIG(inst);  \
    DEVICE_DT_INST_DEFINE (inst, arducam_mega_int_init, NULL, &arducam_mega_data_##inst, &arducam_mega_config_##inst, POST_KERNEL,                             \
                               90, NULL);	

DT_INST_FOREACH_STATUS_OKAY (ARDUCAM_MEGA_DEFINE)