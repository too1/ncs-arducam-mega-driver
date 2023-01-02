#ifndef __UART_HANDLER_H
#define __UART_HANDLER_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>

enum arducam_mega_evt_types_t {APP_UART_EVT_RX, APP_UART_EVT_ERROR, APP_UART_EVT_QUEUE_OVERFLOW};

struct arducam_mega_evt_t {
	enum arducam_mega_evt_types_t type;
	union {
	} data;
};

typedef void (*arducam_mega_event_handler_t)(struct arducam_mega_evt_t *evt);

struct arducam_mega_cfg {
	char *bus_name;
	int (*bus_init) (const struct device *dev);
	struct spi_config spi_conf;
    const char *cs_gpios_label;
};

struct arducam_mega_data {
        const struct device *bus;

        struct spi_cs_control cs_ctrl;
};

int arducam_mega_init(arducam_mega_event_handler_t evt_handler);

#endif