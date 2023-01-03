#include "arducam_nordic_hal.h"

#include <zephyr/drivers/spi.h>

void arducamSpiBegin(void)
{

}   
 
static const struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
				 SPI_MODE_CPOL | SPI_MODE_CPHA,
	.frequency = 4000000,
	.slave = 0,
	.cs = 0,
};

uint8_t arducamSpiTransfer(void *cam, uint8_t tx_data)
{		
	static uint8_t tx_buf[1];
	static uint8_t rx_buf[1];
	tx_buf[0] = tx_data;
	static struct spi_buf s_tx_buf = {
		.buf = tx_buf,
		.len = sizeof(tx_buf)
	};
	static struct spi_buf_set s_tx = {
		.buffers = &s_tx_buf,
		.count = 1
	};
	static struct spi_buf s_rx_buf = {
		.buf = rx_buf,
		.len = sizeof(rx_buf),
	};
	static struct spi_buf_set s_rx = {
		.buffers = &s_rx_buf,
		.count = 1
	};	
	if(spi_transceive(((ArducamCamera *)cam)->spi_dev, &spi_cfg, &s_tx, &s_rx) < 0) {
		return 0;
	}
	return rx_buf[0];
}

void arducamCsOutputMode(int pin)
{

}

void arducamSpiCsPinLow(void *cam)
{
	gpio_pin_set_dt(((ArducamCamera *)cam)->spi_cs_gpio_spec, 1);
}

void arducamSpiCsPinHigh(void *cam)
{
	gpio_pin_set_dt(((ArducamCamera *)cam)->spi_cs_gpio_spec, 0);
}

void arducamDelayMs(int val)
{
	k_msleep(val);
}

void arducamDelayUs(int val)
{
	k_usleep(val);
}