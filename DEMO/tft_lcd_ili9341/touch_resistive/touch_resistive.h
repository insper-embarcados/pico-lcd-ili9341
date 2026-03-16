#ifndef TOUCH_H
#define TOUCH_H

#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define YP_PIN 27
#define YP_CHANNEL 1
#define XM_PIN 26
#define XM_CHANNEL 0

#define XP_PIN 14
#define YM_PIN 20

#define TOUCH_NUMSAMPLES  64
#define TOUCH_LCD_WIDTH   240
#define TOUCH_LCD_HEIGHT  320

#define TOUCH_X_0         3340
#define TOUCH_X_240       550

#define TOUCH_Y_0         510
#define TOUCH_Y_320       3600
#define TOUCH_PRESSURE_DETECT 1000

void configure_touch(void);
int readPoint(int *px, int *py);
void getPoint(int *x, int *y, int *z);

#endif
