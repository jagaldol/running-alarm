#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define BUTTON_PORT               GPIOC            
#define BUTTON_PIN                GPIO_Pin_3


void Init_BUTTON(void);