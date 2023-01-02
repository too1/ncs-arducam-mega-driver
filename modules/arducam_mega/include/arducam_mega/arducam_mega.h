#ifndef __UART_HANDLER_H
#define __UART_HANDLER_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>

enum arducam_mega_evt_types_t {APP_UART_EVT_RX, APP_UART_EVT_ERROR, APP_UART_EVT_QUEUE_OVERFLOW};

struct arducam_mega_evt_t {
	enum arducam_mega_evt_types_t type;
	union {
	} data;
};

typedef void (*arducam_mega_event_handler_t)(struct arducam_mega_evt_t *evt);

int arducam_mega_init(arducam_mega_event_handler_t evt_handler);

#endif