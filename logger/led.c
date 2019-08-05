#include <arch/board/board.h>
#include <arch/chip/pin.h>
#include "led.h"

void led_init(){
  board_gpio_config(LED_0, 0, false, true, 0);
  board_gpio_config(LED_1, 0, false, true, 0);
  board_gpio_config(LED_2, 0, false, true, 0);
  board_gpio_config(LED_3, 0, false, true, 0);
}
void led_write(int led, int value){
  board_gpio_write(led, value);
}
