#include <arch/chip/pin.h>

#define LED_0 PIN_I2S1_DATA_OUT
#define LED_1 PIN_I2S1_DATA_IN
#define LED_2 PIN_I2S1_LRCK
#define LED_3 PIN_I2S1_BCK

void led_init(vold);
void led_write(int led, int value);
