#include <arducam_mega.h>
#include <zephyr/sys/ring_buffer.h>
#include <string.h>

#include <zephyr/logging/log.h>

#define LOG_MODULE_NAME arducam_mega
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

static arducam_mega_event_handler_t app_uart_event_handler;

static const struct device *dev_uart;

int arducam_mega_init(arducam_mega_event_handler_t evt_handler)
{
	int ret;
	LOG_INF("INITIALIZED");
	return 0;
}

void uart_event_thread_func(void)
{
	static struct arducam_mega_evt_t new_event;
}

K_THREAD_DEFINE(app_uart_evt_thread, CONFIG_ARDUCAM_MEGA_EVT_THREAD_STACK_SIZE, uart_event_thread_func, \
				0, 0, 0, CONFIG_ARDUCAM_MEGA_EVT_THREAD_PRIORITY, 0, 100);
