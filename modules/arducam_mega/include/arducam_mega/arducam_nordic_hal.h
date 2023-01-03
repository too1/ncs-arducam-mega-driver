#ifndef __ARDUCAM_NORDIC_HAL_H
#define __ARDUCAM_NORDIC_HAL_H

#include <zephyr/kernel.h>
#include <ArducamCamera.h>

void arducamSpiBegin(void);

uint8_t arducamSpiTransfer(void *cam, uint8_t TxData);
void arducamCsOutputMode(int pin);
void arducamSpiCsPinLow(void *cam, int pin);
void arducamSpiCsPinHigh(void *cam, int pin);
void arducamDelayMs(int val);
void arducamDelayUs(int val);

#endif
