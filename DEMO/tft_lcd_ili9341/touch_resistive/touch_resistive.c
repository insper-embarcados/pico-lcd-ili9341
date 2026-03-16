/*
* touch.c
*
* Created: 10/02/2021 17:42:26
*  Author: Marco Mello
*/

#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "touch_resistive.h"


void configure_touch(void) {
  /*
  * Xm : GPIO 26 - ADC0
  * Ym : GPIO 21
  * Xp : GPIO 22
  * Yp : GPIO 27 - ADC1
  */

    adc_init();

    adc_gpio_init(XM_PIN);
    adc_select_input(XM_CHANNEL);
    
    adc_gpio_init(YP_PIN);
    adc_select_input(YP_CHANNEL);    

}





void configReadTouchX(void){
  gpio_init(YP_PIN);
  gpio_set_dir(YP_PIN, GPIO_IN);

  gpio_init(YM_PIN);
  gpio_set_dir(YM_PIN, GPIO_IN);

  gpio_init(XP_PIN);
  gpio_set_dir(XP_PIN, GPIO_OUT);
  gpio_put(XP_PIN,1);
  sleep_ms(1);  

  gpio_init(XM_PIN); 
  gpio_set_dir(XM_PIN, GPIO_OUT);
  gpio_put(XM_PIN,0);
  sleep_ms(1);

  adc_init();
  gpio_init(YP_PIN);
  adc_gpio_init(YP_PIN);
  adc_select_input(YP_CHANNEL); 
  sleep_ms(1);
}

void configReadTouchY(void) {
  gpio_init(XP_PIN);
  gpio_set_dir(XP_PIN, GPIO_IN);

  gpio_init(XM_PIN);
  gpio_set_dir(XM_PIN, GPIO_IN);

  gpio_init(YP_PIN); 
  gpio_set_dir(YP_PIN, GPIO_OUT);
  gpio_put(YP_PIN,1);
  sleep_ms(1);   

  gpio_init(YM_PIN); 
  gpio_set_dir(YM_PIN, GPIO_OUT);
  gpio_put(YM_PIN,0);
  sleep_ms(1);

  adc_init();
  gpio_init(XM_PIN);
  adc_gpio_init(XM_PIN);
  adc_select_input(XM_CHANNEL);  ////pode ser que seja XM_CHANNEL
  sleep_ms(1);    
}

/*
*  xp to ground
*  ym to vcc
*  HI-Z xm e yp
*/
int readTouchZ(int rxplate) {
  gpio_init(XP_PIN); 
  gpio_set_dir(XP_PIN, GPIO_OUT);
  gpio_put(XP_PIN,0);   

  gpio_init(YM_PIN); 
  gpio_set_dir(YM_PIN, GPIO_OUT);
  gpio_put(YM_PIN,1);
  

  gpio_init(XM_PIN);
  adc_select_input(XM_CHANNEL);

  gpio_init(YP_PIN);
  adc_select_input(YP_CHANNEL);
  sleep_ms(1);


  adc_select_input(XM_CHANNEL);
  sleep_ms(1);
  //configReadTouchY();
  int z1 = adc_read();
  int x = z1;

  adc_select_input(YP_CHANNEL);
  sleep_ms(1);
  //configReadTouchX();
  int z2 = adc_read();


  //configReadTouchX();
  //int x  = adc_read();

  float rtouch;
  rtouch = z2;
  rtouch /= z1;
  rtouch -= 1;
  rtouch *= x;
  rtouch *= rxplate;
  rtouch /= 1024;
  
  // aparecem alguns 0 do nada, evitar!
  if(rtouch < 100.0)
  rtouch = 1024.0;
  return(rtouch);
}


static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;

  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
    array[j] = array[j - 1];
    array[j] = save;
  }
}

void readRawPoint(int *x, int *y, int *z) {
  int samples[TOUCH_NUMSAMPLES];
  
  configReadTouchX();
  //adc_select_input(YP_CHANNEL);
  for (int i=0; i<TOUCH_NUMSAMPLES; i++) {
    sleep_ms(1);
    samples[i] = adc_read();
  }
  insert_sort(samples, TOUCH_NUMSAMPLES);
  *x = (samples[TOUCH_NUMSAMPLES / 2]);
  
  configReadTouchY();
  //adc_select_input(XM_CHANNEL);
  for (int i=0; i<TOUCH_NUMSAMPLES; i++) {
    sleep_ms(1);
    samples[i] = adc_read();
  }
  insert_sort(samples, TOUCH_NUMSAMPLES);
  *y = (samples[TOUCH_NUMSAMPLES / 2]);
  
  *z = readTouchZ(300);
}

// returns if valid touch
// checks Z pressure!
int pointToCoordinate(int x, int y, int z, int *px, int *py) {
  int xtemp, ytemp;
  
  xtemp = TOUCH_LCD_WIDTH*(x-TOUCH_X_240)/(TOUCH_X_0-TOUCH_X_240);
  if(xtemp > TOUCH_LCD_WIDTH) xtemp=TOUCH_LCD_WIDTH;
  if(xtemp < 0) xtemp=0;
  *px = xtemp;
    
  ytemp = TOUCH_LCD_HEIGHT*(y-TOUCH_Y_0)/(TOUCH_Y_320-TOUCH_Y_0);
  if(ytemp > TOUCH_LCD_HEIGHT) ytemp=TOUCH_LCD_HEIGHT;
  if(ytemp < 0) ytemp=0;
  *py = ytemp;

  
  if(z < TOUCH_PRESSURE_DETECT) {
    
    return 1;
  }

  return 0;
}

int readPoint(int *px, int *py){
  int x, y, z;
  readRawPoint(&x, &y, &z);
  //printf("x: %d y: %d z:%d\n", x, y, z);
  return(pointToCoordinate(x, y, z, px, py));
}