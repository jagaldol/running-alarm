#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define US_TIMER               TIM3

// SENSOR PIN
#define US_TRIG_PORT            GPIOB              
#define US_TRIG_PIN               GPIO_Pin_0      //TIM Ch3 (trig output)

#define US_ECHO_PORT            GPIOA
#define US_ECHO_PIN               GPIO_Pin_6      //TIM Ch1 (echo input)
#define US_TIMER_TRIG_SOURCE      TIM_TS_TI1FP1


extern void EnableHCSR04PeriphClock();
void EnableHCSR04PeriphClock();
void InitHCSR04();                                  // MUST DO
int32_t HCSR04GetDistance();                        // USE THIS METHOD TO MEASURE DISTANCE
