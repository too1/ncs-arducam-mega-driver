#ifndef __ARDUCAM_NORDIC_HAL_H
#define __ARDUCAM_NORDIC_HAL_H

#include <zephyr/kernel.h>
#include <ArducamCamera.h>

void arducamSpiBegin(void);

uint8_t arducamSpiTransfer(void *cam, uint8_t TxData);
int arducamSpiFastTransfer(void *cam, uint8_t *tx_buf, uint8_t *rx_buf, uint32_t len);
int arducamSpiFastRead(void *cam, uint8_t *rx_buf, uint32_t len);
void arducamCsOutputMode(int pin);
void arducamSpiCsPinLow(void *cam);
void arducamSpiCsPinHigh(void *cam);
void arducamDelayMs(int val);
void arducamDelayUs(int val);

#endif
