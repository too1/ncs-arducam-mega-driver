**NCS-uart-handler**

Overview
********
This example implements a UART async library that simplifies the interaction with the UART in async mode. 
The library handles the buffering of TX and RX data without relying on dynamic memory allocation, and with a minimal use of memcpy. 

Usage
*****

The library is provided as an out of tree module in Zephyr, and in order to build it the following snippet should be added to the CMakeLists.txt file of the application, where PATH_TO_MODULE is the path to where the module is stored relative to the location of the CMakeLists.txt file:

```c
# Manually add the app_uart module to the sample
list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/PATH_TO_MODULE
  )
```

To enable the module, add the following line to the project configuration (prj.conf normally):

*CONFIG_APP_UART=y*

To select which UART should be used by the app_uart library, it is necessary to add a chosen node to your device tree, assigning your UART of choice to the nordic,app-uart property. As an example, in order to use UART1 for app_uart, add the following lines to your device tree overlay:

```c
/ {
	chosen {
		nordic,app-uart = &uart1;
	};
};
```

Optionally, the following Kconfig parameters can be changed:

*CONFIG_APP_UART_RX_TIMEOUT_US (default 100000)*

	Set the period of inactivity after which any data in the RX buffers will be forwarded to the application. 
	A smaller value will typically lead to more overhead on the processing side, but will ensure quicker response to incoming UART data. 
	Once the current UART buffer fills up the data will be forwarded to the application regardless of the timeout value. 

*CONFIG_APP_UART_TX_BUF_SIZE (default 256)*

	Size of the TX buffer. Set this to 0 to disable TX buffering.
		
*CONFIG_APP_UART_RX_BUF_SIZE (default 256)*

	Size of the RX buffer. The actual UART DMA buffer will be at most half the size of the RX buffer. 

*CONFIG_APP_UART_EVT_QUEUE_SIZE (default 32)*

	Size of the event queue, used to store events such as RX data received or error messages. 
	When using a large RX buffer or a fast RX timeout the event queue might be filled by many small RX messages, and in this case a larger event queue might be necessary. 
	If the event queue overflows an event will be forwarded to the application. 

*CONFIG_APP_UART_EVT_THREAD_STACK_SIZE (default 1024)*

	Stack size of the event handler thread, used to run the application event handler. 

*CONFIG_APP_UART_EVT_THREAD_PRIORITY (default 5)*

	The priority of the event handler thread


The library starts an internal thread that will forward UART events, such as RX data and error conditions, to the application. 
In order to process these events an event handler must be implemented:

```c
static void on_app_uart_event(struct app_uart_evt_t *evt)
{
	switch(evt->type) {
		case APP_UART_EVT_RX:
			// Print the incoming data to the console
			printk("RX ");
			for(int i = 0; i < evt->data.rx.length; i++) printk("%.2x ", evt->data.rx.bytes[i]);
			printk("\n");
			break;

		// A UART error ocurred, such as a break or frame error condition
		case APP_UART_EVT_ERROR:
			printk("UART error: Reason %i\n", evt->data.error.reason);
			break;

		// The UART event queue has overflowed. If this happens consider increasing the UART_EVENT_QUEUE_SIZE (will increase RAM usage),
		// or increase the UART_RX_TIMEOUT_US parameter to avoid a lot of small RX packets filling up the event queue
		case APP_UART_EVT_QUEUE_OVERFLOW:
			printk("UART library error: Event queue overflow!\n");
			break;
	}
}
```

To initialize the library call the init function, and provide a pointer to your event handler:

```c
int err = app_uart_init(on_app_uart_event);
if(err != 0) {
	printk("app_uart_init failed: %i\n", err);
}
```

To send UART data use the app_uart_send(..) function. A timeout parameter can be provided to time out after a predetermined point in time. 

```c
static uint8_t my_data[] = {1,2,3,4,5};
app_uart_send(my_data, sizeof(my_data), K_FOREVER);
```

TODO
****
- Reliability testing
- Throughput testing
- Improved TX handling, including support for no TX buffer
